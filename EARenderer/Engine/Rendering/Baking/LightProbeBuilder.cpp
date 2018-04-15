//
//  LightProbeBuilder.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 18.11.2017.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "LightProbeBuilder.hpp"
#include "Measurement.hpp"
#include "StringUtils.hpp"
#include "Collision.hpp"
#include "trianglepacker.hpp"

#include <stdexcept>

namespace EARenderer {
    
#pragma mark - Lifecycle
    
    LightProbeBuilder::LightProbeBuilder(const Size2D& probeCaptureResolution)
    :
    mEnvironmentMap(probeCaptureResolution),
    mFramebuffer(probeCaptureResolution),
    mDepthRenderbuffer(probeCaptureResolution),
    mDepthCubemap(probeCaptureResolution)
    { }
    
#pragma mark - Private
    
    void LightProbeBuilder::captureEnvironmentForProbe(Scene *scene, const LightProbe& probe) {
        DirectionalLight& directionalLight = scene->directionalLight();
        
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        mEnvironmentCaptureShader.bind();
        mEnvironmentCaptureShader.setLightProbe(probe);
        mEnvironmentCaptureShader.setLight(directionalLight);
        
        for (ID staticInstanceID : scene->staticMeshInstanceIDs()) {
            auto& instance = scene->meshInstances()[staticInstanceID];
            auto& subMeshes = ResourcePool::shared().meshes[instance.meshID()].subMeshes();
            
            mEnvironmentCaptureShader.setModelMatrix(instance.transformation().modelMatrix());
            
            for (ID subMeshID : subMeshes) {
                auto& subMesh = subMeshes[subMeshID];
                auto& material = ResourcePool::shared().materials[instance.materialIDForSubMeshID(subMeshID)];
                
                mEnvironmentCaptureShader.ensureSamplerValidity([this, &material]() {
                    mEnvironmentCaptureShader.setMaterial(material);
                });
                
                subMesh.draw();
            }
        }
    }

    float LightProbeBuilder::surfelSolidAngle(Scene *scene, const Surfel& surfel, const glm::vec3& standpoint) {
        glm::vec3 Wps = surfel.position - standpoint;
        float distance2 = glm::length2(Wps);
        Wps = glm::normalize(Wps);

        float distanceTerm = std::min(surfel.area / distance2, 1.f);
        float visibilityTerm = std::max(glm::dot(-surfel.normal, Wps), 0.f);
        float visibilityTest = 0.0;

        // Save ray casts if surfel's facing away from the standpoint
        if (visibilityTerm > 0.0) {
            visibilityTest = scene->rayTracer()->lineSegmentOccluded(standpoint, surfel.position) ? 0.0 : 1.0;
        }

        return distanceTerm * visibilityTerm * visibilityTest;
    }

    SurfelClusterProjection LightProbeBuilder::projectSurfelCluster(Scene *scene, const SurfelCluster& cluster, const glm::vec3& standpoint) {
        SurfelClusterProjection projection;

        for (size_t i = cluster.surfelOffset; i < cluster.surfelOffset + cluster.surfelCount; i++) {
            Surfel& surfel = scene->surfels()[i];
            glm::vec3 Wps_norm = glm::normalize(surfel.position - standpoint);
            float solidAngle = surfelSolidAngle(scene, surfel, standpoint);

            if (solidAngle > 0.0) {
                projection.sphericalHarmonics.contribute(Wps_norm, surfel.albedo, solidAngle);
            }
        }

        projection.sphericalHarmonics.normalize();

        return projection;
    }

    void LightProbeBuilder::projectSurfelClustersOnProbe(Scene* scene, DiffuseLightProbe& probe) {
        probe.surfelClusterProjectionGroupOffset = scene->surfelClusterProjections().size();

        for (size_t i = 0; i < scene->surfelClusters().size(); i++) {
            SurfelCluster &cluster = scene->surfelClusters()[i];
            SurfelClusterProjection projection = projectSurfelCluster(scene, cluster, probe.position);

            // Only accept projections with non-zero SH
            if (projection.sphericalHarmonics.magnitude2() > 10e-09) {
                projection.surfelClusterIndex = i;
                scene->surfelClusterProjections().push_back(projection);
                probe.surfelClusterProjectionGroupCount++;
            }
        }
    }

#pragma mark - Public interface
    
    void LightProbeBuilder::buildAndPlaceProbesInScene(Scene* scene) {
//        mFramebuffer.bind();
//        mFramebuffer.attachTexture(mEnvironmentMap);
//        mFramebuffer.attachTexture(mDepthCubemap);
//        mFramebuffer.viewport().apply();
//
//        AxisAlignedBox3D bb = scene->boundingBox();
//        glm::vec3 step { bb.max.x - bb.min.x, bb.max.y - bb.min.y, bb.max.z - bb.min.z };
//        step /= mSpaceDivisionResolution;
//
//        for (int32_t x = bb.min.x; x < bb.max.x; x += step.x) {
//            for (int32_t y = bb.min.y; y < bb.max.y; y += step.y) {
//                for (int32_t z = bb.min.z; z < bb.max.z; z += step.z) {
//                    const auto& probe = LightProbe({ x, y, z });
//                    captureEnvironmentForProbe(scene, probe);
//                    scene->lightProbes().emplace(probe);
//                }
//            }
//        }
    }

    void LightProbeBuilder::buildDynamicGeometryProbes(Scene *scene) {
        AxisAlignedBox3D bb = scene->lightBakingVolume();
        glm::vec3 step = bb.max - bb.min;
        glm::uvec3 resolution = scene->preferredProbeGridResolution();
        step.x /= resolution.x - 1;
        step.y /= resolution.y - 1;
        step.z /= resolution.z - 1;

        printf("Building grid probes...\n");
        Measurement::executionTime("Grid probes placement took", [&]() {
             for (float z = bb.min.z; z <= bb.max.z + step.z / 2.0; z += step.z) {
                for (float y = bb.min.y; y <= bb.max.y + step.y / 2.0; y += step.y) {
                    for (float x = bb.min.x; x <= bb.max.x + step.x / 2.0; x += step.x) {
                        DiffuseLightProbe probe({ x, y, z });
                        projectSurfelClustersOnProbe(scene, probe);
                        scene->diffuseLightProbes().push_back(probe);
                    }
                }
            }
            printf("Built %lu probes | %lu projections \n", scene->diffuseLightProbes().size(), scene->surfelClusterProjections().size());
        });
    }

    void LightProbeBuilder::buildStaticGeometryProbes(Scene *scene) {
        glm::mat4 lightBakingVolumeLocalSpace = scene->lightBakingVolume().localSpaceMatrix();
        Size2D lightMapResolution = scene->preferredProbeLightmapResolution();
        glm::vec2 glmResolution = glm::vec2(lightMapResolution.width, lightMapResolution.height);

        const uint32_t kInvalidIndex = -1;
        std::vector<uint32_t> probeIndices;
        probeIndices.assign(lightMapResolution.width * lightMapResolution.height, kInvalidIndex);

        printf("Building lightmapped probes...\n");
        Measurement::executionTime("Lightmapped probes placement took", [&]() {

            std::vector<glm::vec3> vertices;
            std::vector<uint16_t> indices;

            uint16_t index = 0;

            printf("Collecting triangles... \n");

            for (ID meshInstanceID : scene->staticMeshInstanceIDs()) {
                auto& meshInstance = scene->meshInstances()[meshInstanceID];
                auto& mesh = ResourcePool::shared().meshes[meshInstance.meshID()];

                auto modelMatrix = meshInstance.modelMatrix();

                for (ID subMeshID : mesh.subMeshes()) {
                    auto& subMesh = mesh.subMeshes()[subMeshID];

                    for (size_t i = 0; i < subMesh.vertices().size(); i += 3) {
                        Triangle3D triangle(modelMatrix * subMesh.vertices()[i].position,
                                            modelMatrix * subMesh.vertices()[i + 1].position,
                                            modelMatrix * subMesh.vertices()[i + 2].position);

                        if (Collision::TriangleAABB(triangle, scene->lightBakingVolume())) {
                            vertices.emplace_back(triangle.p1);
                            indices.push_back(index);
                            index++;

                            vertices.emplace_back(triangle.p2);
                            indices.push_back(index);
                            index++;

                            vertices.emplace_back(triangle.p3);
                            indices.push_back(index);
                            index++;
                        }
                    }

//
//
//
//
//                        glm::vec3 p0Norm = lightBakingVolumeLocalSpace * glm::vec4(p0, 1.0);
//                        glm::vec2 uv0 = GLTexture::UVMap(p0Norm, subMesh.vertices()[i].normal);
//                        glm::ivec2 uv0i = uv0 * glmResolution;
//
//
//                        glm::vec3 p1Norm = lightBakingVolumeLocalSpace * glm::vec4(p1, 1.0);
//                        glm::vec2 uv1 = GLTexture::UVMap(p1Norm, subMesh.vertices()[i + 1].normal);
//                        glm::ivec2 uv1i = uv1 * glmResolution;
//
//
//                        glm::vec3 p2Norm = lightBakingVolumeLocalSpace * glm::vec4(p2, 1.0);
//                        glm::vec2 uv2 = GLTexture::UVMap(p2Norm, subMesh.vertices()[i + 2].normal);
//                        glm::ivec2 uv2i = uv2 * glmResolution;
//
//                        glm::vec2 minUV = glm::min(glm::min(uv0, uv1), uv2);
//                        glm::vec2 maxUV = glm::max(glm::max(uv0, uv1), uv2);
//
//                        glm::vec2 deltaUV = maxUV - minUV;
//
//
//                        glm::ivec2 iMinUV = minUV * glmResolution;
//                        glm::ivec2 iMaxUV = maxUV * glmResolution;
//
//                        iMinUV = glm::clamp(iMinUV, glm::ivec2(0), glm::ivec2(glmResolution));
//                        iMaxUV = glm::clamp(iMaxUV, glm::ivec2(0), glm::ivec2(glmResolution));
//
//                        for (size_t v = iMinUV.y; v < iMaxUV.y; v++) {
//                            for (size_t u = iMinUV.x; u < iMaxUV.x; u++) {
//                                size_t flatIndex = u + v * lightMapResolution.width;
//
//                                if (probeIndices[flatIndex] == kInvalidIndex) {
//                                    Triangle3D UVs(glm::vec3(uv0i, 0.0), glm::vec3(uv1i, 0.0), glm::vec3(uv2i, 0.0));
//                                    glm::vec3 barycentric = Collision::Barycentric(glm::vec3(u, v, 0.0), UVs);
//                                    bool pointInside = barycentric.x > 0.0 && barycentric.y > 0.0 && barycentric.z > 0.0;
//
//                                    if (pointInside) {
//                                        probeIndices[flatIndex] = 0;
//                                        glm::vec3 probePosition = p0 * barycentric.x + p1 * barycentric.y + p2 * barycentric.z;
//                                        printf("Inserting probe at %f %f %f \n", probePosition.x, probePosition.y, probePosition.z);
//
//                                        DiffuseLightProbe probe(probePosition);
//                                        scene->diffuseLightProbes().push_back(probe);
//                                    }
//                                }
//                            }
//                        }


//                    }


                }
            }

//            printf("UV mapping... \n");
//
//            std::size_t count = 0;
//
//            std::vector<std::uint16_t> remap(indices.size()); // allocate buffer for each vertex index
//            std::vector<float> uvs(indices.size() * 2); // allocate buffer for each output uv
//            std::vector<std::uint16_t> outIndices(indices.size()); // allocate buffer for each output uv
//
//            ray::uvmapper::lightmappack((float *)vertices.data(), indices.data(), indices.size(),
//                                        lightMapResolution.width, lightMapResolution.height, 0,
//                                        remap.data(), uvs.data(), outIndices.data(), count);
//
//            printf("");
        });
    }
    
}
