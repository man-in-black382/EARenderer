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

        // Used to indicate whether surfel is located in the positive hemisphere of static geometry probe
        // to reduce ray casts. For full-sphere (dynamic geometry) grid probes it is always true.
        bool isInSameHemisphere = true;

        if (probe.normal.x != 0.0 || probe.normal.y != 0.0 || probe.normal.z != 0.0) {
            isInSameHemisphere = glm::dot(Wps, probe.normal) > 0.0;
        }

        float visibilityTest = 0.0;

        // Save ray casts if surfel's facing away from the standpoint
        if (visibilityTerm > 0.0 && isInSameHemisphere) {
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
        size_t alignedHeight = texelCount / alignedWidth;

        mOcclusionTextureResolution = Size2D(alignedWidth, alignedHeight);
    }

    void DiffuseLightProbeGenerator::findOcclusionsDistancesForProbe(int32_t probeIndex) {

        const int32_t kFaceCount = 6;

        auto& probe = mProbeData->probes()[probeIndex];
        int32_t probeShadowMapsPerRow = mOcclusionTextureResolution.width / mOcclusionMapFaceResolution.width / 6;
        int32_t yOffset = probeIndex / probeShadowMapsPerRow;
        int32_t xOffset = probeIndex % probeShadowMapsPerRow;

        int32_t xOffsetInPixels = xOffset * mOcclusionMapFaceResolution.width * kFaceCount;

        printf("Probe idx: %d | X offset: %d | Y offset: %d \n", probeIndex, xOffset, yOffset);

        for (int32_t cubeFaceIndex = 0; cubeFaceIndex < kFaceCount; cubeFaceIndex++) {

            GLCubemapFace face = CubemapFaceFromIndex(cubeFaceIndex);

            for (int32_t localY = 0; localY < mOcclusionMapFaceResolution.height; localY++) {

                for (int32_t localX = 0; localX < mOcclusionMapFaceResolution.width; localX++) {

                    int32_t globalX = localX + mOcclusionMapFaceResolution.width * cubeFaceIndex + xOffsetInPixels;
                    int32_t globalY = localY + mOcclusionMapFaceResolution.height * yOffset;

                    glm::vec3 direction;
                    GLCubemapSampler::ComputeSampleVector(face, localX, localY, mOcclusionMapFaceResolution, direction);

                    float hitDistance = std::numeric_limits<float>::max();
                    Ray3D ray(probe.position, direction);

                    if (mScene->rayTracer()->rayHit(ray, hitDistance)) {
//                        printf("Texture coords: %d %d \n", globalX, globalY);

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

    std::array<std::vector<glm::uvec3>, 6> DiffuseLightProbeGenerator::cubemapTextureCoordinates() const {
        std::array<std::vector<glm::uvec3>, 6> coordinates;

        for (size_t i = 0; i < 6; i++) {
            coordinates[i].assign(mOcclusionMapFaceResolution.width * mOcclusionMapFaceResolution.height, glm::zero<glm::vec3>());
            for (size_t y = 0; y < mOcclusionMapFaceResolution.height; y++) {
                for (size_t x = 0; x < mOcclusionMapFaceResolution.width; x++) {
                    size_t flatIndex = y * mOcclusionMapFaceResolution.width + x;
                    coordinates[i][flatIndex] = glm::uvec3(i, x, y);
                }
            }
        }

        return coordinates;
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
                    }
                }
            }
            printf("Built %lu probes | %lu projections \n", mProbeData->mProbes.size(), mProbeData->mSurfelClusterProjections.size());
        });

        mProbeData->mProjectionClusterSHsBufferTexture = std::make_shared<GLFloat3BufferTexture<SphericalHarmonics>>();
        mProbeData->mProjectionClusterSHsBufferTexture->buffer().initialize(surfelProjectionsSH());
        
        mProbeData->mProjectionClusterIndicesBufferTexture = std::make_shared<GLUIntegerBufferTexture<uint32_t>>();
        mProbeData->mProjectionClusterIndicesBufferTexture->buffer().initialize(surfelClusterIndices());

        mProbeData->mProbeClusterProjectionsMetadataBufferTexture = std::make_shared<GLUIntegerBufferTexture<uint32_t>>();
        mProbeData->mProbeClusterProjectionsMetadataBufferTexture->buffer().initialize(probeProjectionsMetadata());

        mOcclusionMapFaceResolution = occlusionMapResolution;
        calculateOcclusionTextureResolution();
        mOcclusionDistances.assign(mOcclusionTextureResolution.width * mOcclusionTextureResolution.height, 0);

        Measurement::ExecutionTime("Occlusion maps generation took", [&]() {
            for (int32_t i = 0; i < mProbeData->probes().size(); i++) {
                findOcclusionsDistancesForProbe(i);
            }
        });

        mProbeData->mOcclusionMapAtlas = std::make_shared<GLHDRTexture2D>(mOcclusionDistances, mOcclusionTextureResolution);
        mProbeData->mCubeFaceTextureCoordsMap = std::make_shared<GLLDRTextureCubemap>(cubemapTextureCoordinates());

        return mProbeData;
    }

}
