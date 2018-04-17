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

    float LightProbeBuilder::surfelSolidAngle(Scene *scene, const Surfel& surfel, const DiffuseLightProbe& probe) {
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
            visibilityTest = scene->rayTracer()->lineSegmentOccluded(probe.position, surfel.position) ? 0.0 : 1.0;
        }

        return distanceTerm * visibilityTerm * visibilityTest;
    }

    SurfelClusterProjection LightProbeBuilder::projectSurfelCluster(Scene *scene, const SurfelCluster& cluster, const DiffuseLightProbe& probe) {
        SurfelClusterProjection projection;

        for (size_t i = cluster.surfelOffset; i < cluster.surfelOffset + cluster.surfelCount; i++) {
            Surfel& surfel = scene->surfels()[i];
            glm::vec3 Wps_norm = glm::normalize(surfel.position - probe.position);
            float solidAngle = surfelSolidAngle(scene, surfel, probe);

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
            SurfelClusterProjection projection = projectSurfelCluster(scene, cluster, probe);

            // Only accept projections with non-zero SH
            if (projection.sphericalHarmonics.magnitude2() > 10e-09) {
                projection.surfelClusterIndex = i;
                scene->surfelClusterProjections().push_back(projection);
                probe.surfelClusterProjectionGroupCount++;
            }
        }
    }

    void LightProbeBuilder::generateProbesForStaticVertices(Scene *scene,
                                                            const glm::vec2& lightmapResolution,
                                                            const glm::mat4& modelMatrix,
                                                            const Vertex1P1N2UV1T1BT& vertex0,
                                                            const Vertex1P1N2UV1T1BT& vertex1,
                                                            const Vertex1P1N2UV1T1BT& vertex2)
    {
        // All texel centers must be found on the geometry to be placed in 3D.
        // Therefore, we iterate over all light-mapped triangles in the scene once and locate the texel centers.
        // For a triangle, the bounding rectangle is determined in texture space,
        // and each of the texels is tested if it is within the triangle.
        // If so, the position of the cache is computed by the texel’s position within the triangle.
        // Texels in empty areas of the light map are not filled by this process.

        Triangle3D triangle(modelMatrix * vertex0.position, modelMatrix * vertex1.position, modelMatrix * vertex2.position);

        if (!Collision::TriangleAABB(triangle, scene->lightBakingVolume())) {
            return;
        }

        glm::ivec2 uv0i = vertex0.lightmapCoords * lightmapResolution;
        glm::ivec2 uv1i = vertex1.lightmapCoords * lightmapResolution;
        glm::ivec2 uv2i = vertex2.lightmapCoords * lightmapResolution;

        glm::vec2 minUV = glm::min(glm::min(vertex0.lightmapCoords, vertex1.lightmapCoords), vertex2.lightmapCoords);
        glm::vec2 maxUV = glm::max(glm::max(vertex0.lightmapCoords, vertex1.lightmapCoords), vertex2.lightmapCoords);

        glm::ivec2 iMinUV = minUV * lightmapResolution;
        glm::ivec2 iMaxUV = maxUV * lightmapResolution;

        iMinUV = glm::clamp(iMinUV, glm::ivec2(0), glm::ivec2(lightmapResolution));
        iMaxUV = glm::clamp(iMaxUV, glm::ivec2(0), glm::ivec2(lightmapResolution));

        for (size_t v = iMinUV.y; v < iMaxUV.y; v++) {
            for (size_t u = iMinUV.x; u < iMaxUV.x; u++) {
                size_t flatIndex = u + v * lightmapResolution.x;

                if (scene->diffuseProbeLightmapIndices()[flatIndex] == -1) {
                    Triangle3D UVs(glm::vec3(uv0i, 0.0), glm::vec3(uv1i, 0.0), glm::vec3(uv2i, 0.0));
                    glm::vec3 barycentric = Collision::Barycentric(glm::vec3(u, v, 0.0), UVs);
                    bool pointInside = barycentric.x > 0.0 && barycentric.y > 0.0 && barycentric.z > 0.0;

                    if (pointInside) {
                        scene->diffuseProbeLightmapIndices()[flatIndex] = 0;
                        glm::vec3 probePosition = triangle.p1 * barycentric.x + triangle.p2 * barycentric.y + triangle.p3 * barycentric.z;
                        glm::vec3 probeNormal = vertex0.normal * barycentric.x + vertex1.normal * barycentric.y + vertex2.normal * barycentric.z;

//                        printf("Normal 1: %f %f %f \n", vertex0.normal.x, vertex0.normal.y, vertex0.normal.z);
//                        printf("Normal 2: %f %f %f \n", vertex0.normal.x, vertex0.normal.y, vertex0.normal.z);
//                        printf("Normal 3: %f %f %f \n", vertex0.normal.x, vertex0.normal.y, vertex0.normal.z);
//                        printf("Interp: %f %f %f \n\n", probeNormal.x, probeNormal.y, probeNormal.z);

                        DiffuseLightProbe probe(probePosition);
                        probe.normal = glm::normalize(probeNormal);
                        projectSurfelClustersOnProbe(scene, probe);
                        scene->diffuseLightProbes().push_back(probe);
                    }
                }
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
        Size2D lightMapResolution = scene->preferredProbeLightmapResolution();
        glm::vec2 glmResolution = glm::vec2(lightMapResolution.width, lightMapResolution.height);

        const int32_t kInvalidIndex = -1;
        std::vector<int32_t> probeIndices;
        probeIndices.assign(lightMapResolution.width * lightMapResolution.height, kInvalidIndex);
        scene->setDiffuseProbeLightmapIndices(probeIndices);

        printf("Building lightmapped probes...\n");

        Measurement::executionTime("Lightmapped probes placement took", [&]() {
            size_t probeOffset = scene->diffuseLightProbes().size();

            for (ID meshInstanceID : scene->staticMeshInstanceIDs()) {
                auto& meshInstance = scene->meshInstances()[meshInstanceID];
                auto& mesh = ResourcePool::shared().meshes[meshInstance.meshID()];

                auto modelMatrix = meshInstance.modelMatrix();

                for (ID subMeshID : mesh.subMeshes()) {
                    auto& subMesh = mesh.subMeshes()[subMeshID];

                    for (size_t i = 0; i < subMesh.vertices().size(); i += 3) {
                        auto& v0 = subMesh.vertices()[i];
                        auto& v1 = subMesh.vertices()[i + 1];
                        auto& v2 = subMesh.vertices()[i + 2];

                        generateProbesForStaticVertices(scene, glmResolution, modelMatrix, v0, v1, v2);
                    }
                }
            }

            printf("Built %ld static geometry probes \n", scene->diffuseLightProbes().size() - probeOffset);
        });
    }
    
}
