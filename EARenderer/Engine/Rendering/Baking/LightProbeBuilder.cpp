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

namespace EARenderer {
    
#pragma mark - Lifecycle
    
    LightProbeBuilder::LightProbeBuilder(const Size2D& probeCaptureResolution, uint32_t spaceDivisionResolution)
    :
    mEnvironmentMap(probeCaptureResolution),
    mFramebuffer(probeCaptureResolution),
    mDepthRenderbuffer(probeCaptureResolution),
    mDepthCubemap(probeCaptureResolution),
    mSpaceDivisionResolution(spaceDivisionResolution)
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
//            visibilityTest = scene->rayTracer()->lineSegmentOccluded(standpoint, surfel.position) ? 0.0 : 1.0;
        }

        return distanceTerm * visibilityTerm;// * visibilityTest;
    }

    SurfelClusterProjection LightProbeBuilder::projectSurfelCluster(Scene *scene, const SurfelCluster& cluster, const glm::vec3& standpoint) {
        SurfelClusterProjection projection;

        for (size_t i = cluster.surfelOffset; i < cluster.surfelOffset + cluster.surfelCount; i++) {
            Surfel& surfel = scene->surfels()[i];
            glm::vec3 Wps_norm = glm::normalize(surfel.position - standpoint);
            float solidAngle = surfelSolidAngle(scene, surfel, standpoint);

            if (solidAngle > 0.0) {
                projection.sphericalHarmonics.contribute(Wps_norm,
//                                                         glm::vec3(1.0, 0.0, 0.0),
                                                         surfel.albedo,
                                                         solidAngle);
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

//        printf("Projected clusters on probe at %f %f %f\n", probe.position.x, probe.position.y, probe.position.z);
//        printf("Clusters offset: %zu | count: %zu \n\n", probe.surfelClusterProjectionGroupOffset, probe.surfelClusterProjectionGroupCount);
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

        // DEBUG
        
        LightProbe probe(glm::vec3(-1.922861, -1.135728, 0.476772), 100);
        captureEnvironmentForProbe(scene, probe);
        probe.updateSHCoefficients(mEnvironmentMap);
        scene->lightProbes().emplace(probe);

        scene->sphericalHarmonicsBufferTexture().buffer().initialize(nullptr, 1);
        scene->sphericalHarmonicsBufferTexture().buffer().write([&probe](auto writer) {
            writer.writeAt(0, probe.sphericalHarmonics());
        });
    }

    void LightProbeBuilder::buildAndPlaceProbesForDynamicGeometry(Scene *scene) {
        AxisAlignedBox3D bb = scene->lightBakingVolume();
        glm::vec3 step = (bb.max - bb.min) / (float)mSpaceDivisionResolution;

        printf("Building grid probes...\n");
        Measurement::executionTime("Grid probes placement took", [&]() {
             for (float z = bb.min.z; z <= bb.max.z; z += step.z) {
                for (float y = bb.min.y; y <= bb.max.y; y += step.y) {
                    for (float x = bb.min.x; x <= bb.max.x; x += step.x) {
                        DiffuseLightProbe probe({ x, y, z });
                        projectSurfelClustersOnProbe(scene, probe);
                        scene->diffuseLightProbes().push_back(probe);
                    }
                }
            }
            printf("Built %lu probes | %lu projections \n", scene->diffuseLightProbes().size(), scene->surfelClusterProjections().size());
        });
    }
    
}
