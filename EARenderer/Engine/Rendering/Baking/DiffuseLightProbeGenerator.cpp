//
//  DiffuseLightProbeGenerator.cpp
//  EARenderer
//
//  Created by Pavel Muratov on 5/3/18.
//  Copyright © 2018 MPO. All rights reserved.
//

#include "DiffuseLightProbeGenerator.hpp"
#include "Measurement.hpp"

namespace EARenderer {

#pragma mark - Protected

    float DiffuseLightProbeGenerator::surfelSolidAngle(const Surfel& surfel, const DiffuseLightProbe& probe) {
        glm::vec3 Wps = surfel.position - probe.position;
        float distance2 = glm::length2(Wps);
        Wps = glm::normalize(Wps);

        float distanceTerm = std::min(surfel.area / distance2, 1.f);

        float visibilityTerm = std::max(glm::dot(-surfel.normal, Wps), 0.f);

        float visibilityTest = 0.0;

        // Save ray casts if surfel's facing away from the standpoint
        if (visibilityTerm > 0.0) {
            visibilityTest = mScene->rayTracer()->lineSegmentOccluded(probe.position, surfel.position) ? 0.0 : 1.0;
        }

        return distanceTerm * visibilityTerm * visibilityTest;
    }

    SurfelClusterProjection DiffuseLightProbeGenerator::projectSurfelCluster(const SurfelCluster& cluster, const DiffuseLightProbe& probe) {
        SurfelClusterProjection projection;

        for (size_t i = cluster.surfelOffset; i < cluster.surfelOffset + cluster.surfelCount; i++) {
            const Surfel& surfel = mSurfelData->surfels()[i];
            glm::vec3 Wps_norm = glm::normalize(surfel.position - probe.position);
            float solidAngle = surfelSolidAngle(surfel, probe);

            if (solidAngle > 0.0) {

//                float Co = (albedo.r - albedo.b) / 2.0;
//                float t = albedo.b + Co;
//                float Cg = (albedo.g - t) / 2.0;
//                float Y = t + Cg;
//
//                glm::vec3 YCoCg(Y, Co, Cg);

                projection.sphericalHarmonics.contribute(Wps_norm, surfel.albedo, solidAngle);
            }
        }

        projection.sphericalHarmonics.normalize();

        return projection;
    }

    void DiffuseLightProbeGenerator::projectSurfelClustersOnProbe(DiffuseLightProbe& probe) {
        probe.surfelClusterProjectionGroupOffset = mProbeData->mSurfelClusterProjections.size();

        for (size_t i = 0; i < mSurfelData->surfelClusters().size(); i++) {
            const SurfelCluster &cluster = mSurfelData->surfelClusters()[i];
            SurfelClusterProjection projection = projectSurfelCluster(cluster, probe);

            // Only accept projections with non-zero SH
            if (projection.sphericalHarmonics.magnitude2() > 10e-09) {
                projection.surfelClusterIndex = i;
                mProbeData->mSurfelClusterProjections.push_back(projection);
                probe.surfelClusterProjectionGroupCount++;
            }
        }
    }

    void DiffuseLightProbeGenerator::calculateOcclusionTextureResolution() {
        size_t probeCount = mProbeData->probes().size();
        size_t occlusionMapFaceCount = 6;

        size_t texelCount = mOcclusionMapFaceResolution.width * mOcclusionMapFaceResolution.height * probeCount * occlusionMapFaceCount;

        Size2D estimatedSize = GLTexture::EstimatedSize(texelCount);

        size_t tail = (size_t)estimatedSize.width % size_t(occlusionMapFaceCount * mOcclusionMapFaceResolution.width);
        size_t alignedWidth = estimatedSize.width - tail;
        size_t alignedHeight = texelCount / alignedWidth + mOcclusionMapFaceResolution.height;

        mOcclusionTextureResolution = Size2D(alignedWidth, alignedHeight);
    }

    void DiffuseLightProbeGenerator::findOcclusionsDistancesForProbe(int32_t probeIndex) {

        const int32_t kFaceCount = 6;

        auto& probe = mProbeData->probes()[probeIndex];
        int32_t probeShadowMapsPerRow = mOcclusionTextureResolution.width / mOcclusionMapFaceResolution.width / 6;
        int32_t yOffset = probeIndex / probeShadowMapsPerRow;
        int32_t xOffset = probeIndex % probeShadowMapsPerRow;

        int32_t xOffsetInPixels = xOffset * mOcclusionMapFaceResolution.width * kFaceCount;
        int32_t yOffsetInPixels = yOffset * mOcclusionMapFaceResolution.height;

        mProbeOcclusionMapAtlasOffsets.emplace_back(xOffsetInPixels, yOffsetInPixels);
        printf("Added offset x: %d | y: %d \n", xOffsetInPixels, yOffsetInPixels);

//        printf("Probe idx: %d | X offset: %d | Y offset: %d \n", probeIndex, xOffset, yOffset);

        for (int32_t cubeFaceIndex = 0; cubeFaceIndex < kFaceCount; cubeFaceIndex++) {

            GLCubemapFace face = CubemapFaceFromIndex(cubeFaceIndex);

            for (int32_t localY = 0; localY < mOcclusionMapFaceResolution.height; localY++) {

                for (int32_t localX = 0; localX < mOcclusionMapFaceResolution.width; localX++) {

                    int32_t globalX = localX + mOcclusionMapFaceResolution.width * cubeFaceIndex + xOffsetInPixels;
                    int32_t globalY = localY + yOffsetInPixels;

                    glm::vec3 direction;
                    GLCubemapSampler::ComputeSampleVector(face, localX, localY, mOcclusionMapFaceResolution, direction);

//                    printf("Direction for %d %d - %f %f %f \n", localX, localY, direction.x, direction.y, direction.z);

                    glm::vec2 testCoords;
                    GLCubemapFace testFace;
                    GLHDRCubemapSampler::ComputeTexCoords(direction, testFace, testCoords);

//                    printf("Coords for direction %f %f %f - %f %f \n\n", direction.x, direction.y, direction.z, testCoords.x, testCoords.y);

                    float hitDistance = std::numeric_limits<float>::max();
                    Ray3D ray(probe.position, direction);

                    if (mScene->rayTracer()->rayHit(ray, hitDistance, EmbreeRayTracer::FaceFilter::CullFront)) {
                        int32_t flatIndex = globalY * mOcclusionTextureResolution.width + globalX;
                        mOcclusionDistances[flatIndex] = hitDistance;
                    }
                }
            }
        }
    }

    std::vector<SphericalHarmonics> DiffuseLightProbeGenerator::surfelProjectionsSH() const {
        std::vector<SphericalHarmonics> shs;
        for (auto& projection : mProbeData->mSurfelClusterProjections) {
            shs.push_back(projection.sphericalHarmonics);
        }
        return shs;
    }

    std::vector<uint32_t> DiffuseLightProbeGenerator::surfelClusterIndices() const {
        std::vector<uint32_t> indices;
        for (auto& projection : mProbeData->mSurfelClusterProjections) {
            indices.push_back(static_cast<uint32_t>(projection.surfelClusterIndex));
        }
        return indices;
    }

    std::vector<uint32_t> DiffuseLightProbeGenerator::probeProjectionsMetadata() const {
        std::vector<uint32_t> metadata;
        for (auto& probe : mProbeData->mProbes) {
            metadata.push_back((uint32_t)probe.surfelClusterProjectionGroupOffset);
            metadata.push_back((uint32_t)probe.surfelClusterProjectionGroupCount);
        }
        return metadata;
    }

#pragma mark - Public interface

    std::shared_ptr<DiffuseLightProbeData> DiffuseLightProbeGenerator::generateProbes(const Scene *scene, std::shared_ptr<const SurfelData> surfelData, const Size2D& occlusionMapResolution) {
        mProbeData = std::make_shared<DiffuseLightProbeData>();
        mSurfelData = surfelData;
        mScene = scene;

        AxisAlignedBox3D bb = scene->lightBakingVolume();
        glm::vec3 step = bb.max - bb.min;
        glm::uvec3 resolution = scene->preferredProbeGridResolution();
        step.x /= resolution.x - 1;
        step.y /= resolution.y - 1;
        step.z /= resolution.z - 1;

        printf("Building grid probes...\n");
        Measurement::ExecutionTime("Grid probes placement took", [&]() {
            for (float z = bb.min.z; z <= bb.max.z + step.z / 2.0; z += step.z) {
                for (float y = bb.min.y; y <= bb.max.y + step.y / 2.0; y += step.y) {
                    for (float x = bb.min.x; x <= bb.max.x + step.x / 2.0; x += step.x) {
                        DiffuseLightProbe probe({ x, y, z });
                        projectSurfelClustersOnProbe(probe);
                        mProbeData->mProbes.push_back(probe);
                        mProbePositions.emplace_back(x, y, z);
                    }
                }
            }

//            DiffuseLightProbe probe0({ 0.1, 0.1, 0.1 });
//            projectSurfelClustersOnProbe(probe0);
//            mProbeData->mProbes.push_back(probe0);
//            mProbePositions.emplace_back(0.1, 0.1, 0.1);
//
//            DiffuseLightProbe probe1({ 0.5, 0.5, 0.5 });
//            projectSurfelClustersOnProbe(probe1);
//            mProbeData->mProbes.push_back(probe1);
//            mProbePositions.emplace_back(0.5, 0.5, 0.5);
//
//            DiffuseLightProbe probe2({ 0.8, 0.8, 0.8 });
//            projectSurfelClustersOnProbe(probe2);
//            mProbeData->mProbes.push_back(probe2);
//            mProbePositions.emplace_back(0.8, 0.8, 0.8);

            printf("Built %lu probes | %lu projections \n", mProbeData->mProbes.size(), mProbeData->mSurfelClusterProjections.size());
        });

        mProbeData->mProjectionClusterSHsBufferTexture = std::make_shared<GLFloat3BufferTexture<SphericalHarmonics>>();
        mProbeData->mProjectionClusterSHsBufferTexture->buffer().initialize(surfelProjectionsSH());
        
        mProbeData->mProjectionClusterIndicesBufferTexture = std::make_shared<GLUIntegerBufferTexture<uint32_t>>();
        mProbeData->mProjectionClusterIndicesBufferTexture->buffer().initialize(surfelClusterIndices());

        mProbeData->mProbeClusterProjectionsMetadataBufferTexture = std::make_shared<GLUIntegerBufferTexture<uint32_t>>();
        mProbeData->mProbeClusterProjectionsMetadataBufferTexture->buffer().initialize(probeProjectionsMetadata());

//        mOcclusionMapFaceResolution = occlusionMapResolution;
//        calculateOcclusionTextureResolution();
//        mOcclusionDistances.assign(mOcclusionTextureResolution.width * mOcclusionTextureResolution.height, std::numeric_limits<float>::max());

//        Measurement::ExecutionTime("Occlusion maps generation took", [&]() {
//            for (int32_t i = 0; i < mProbeData->probes().size(); i++) {
//                findOcclusionsDistancesForProbe(i);
//            }
//        });

//        mProbeData->mOcclusionMapAtlas = std::make_shared<GLHDRTexture2D>(mOcclusionDistances, mOcclusionTextureResolution);

//        mProbeData->mOcclusionMapAtlasOffsetsBufferTexture = std::make_shared<GLUInteger2BufferTexture<glm::uvec2>>();
//        mProbeData->mOcclusionMapAtlasOffsetsBufferTexture->buffer().initialize(mProbeOcclusionMapAtlasOffsets);

        mProbeData->mProbePositionsBufferTexture = std::make_shared<GLFloat3BufferTexture<glm::vec3>>();
        mProbeData->mProbePositionsBufferTexture->buffer().initialize(mProbePositions);

        return mProbeData;
    }

}
