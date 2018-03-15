//
//  LightProbeBuilder.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 18.11.2017.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "LightProbeBuilder.hpp"

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
        ID directionalLightID = *(scene->directionalLights().begin());
        DirectionalLight& directionalLight = scene->directionalLights()[directionalLightID];
        
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

    float LightProbeBuilder::surfelClusterSolidAngle(Scene *scene, const SurfelCluster& cluster, const glm::vec3& standpoint) {
        float solidAngle = 0.0;

        for (size_t i = cluster.surfelOffset; i < cluster.surfelOffset + cluster.surfelCount; i++) {
            const Surfel& surfel = scene->surfels()[i];

            glm::vec3 Wps = surfel.position - standpoint;
            float area2 = surfel.area * surfel.area;
            float distance2 = glm::length2(Wps);

            float distanceTerm = std::min(area2 / distance2, 1.f);
            float visibilityTerm = std::max(glm::dot(-surfel.normal, Wps), 0.f);
            float visibilityTest = 0.0;

            // Save ray casts if surfel's facing away from the standpoint
            if (visibilityTerm > 0.0) {
                visibilityTest = scene->octree().raymarch(standpoint, surfel.position) ? 1.0 : 0.0;
            }
        }

        return solidAngle;
    }

    SurfelClusterProjection LightProbeBuilder::projectSurfelCluster(Scene *scene, const SurfelCluster& cluster, const glm::vec3& standpoint) {
        SurfelClusterProjection projection;

        return projection;
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
        glm::vec3 step { bb.max.x - bb.min.x, bb.max.y - bb.min.y, bb.max.z - bb.min.z };
        step /= mSpaceDivisionResolution;

        for (float x = bb.min.x; x < bb.max.x; x += step.x) {
            for (float y = bb.min.y; y < bb.max.y; y += step.y) {
                for (float z = bb.min.z; z < bb.max.z; z += step.z) {



                }
            }
        }
    }
    
}
