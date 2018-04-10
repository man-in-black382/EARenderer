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
        glm::vec3 step = (bb.max - bb.min) / (float)(mSpaceDivisionResolution - 1);

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

//        SphericalHarmonics greenWhiteSH;
//        greenWhiteSH.mL00  = glm::vec3(1.77245402, 3.54490805, 1.77245402);
//        greenWhiteSH.mL11  = glm::vec3(3.06998014, 0.0, 3.06998014);
//        greenWhiteSH.mL10  = glm::vec3(0.0);
//        greenWhiteSH.mL1_1 = glm::vec3(0.0);
//        greenWhiteSH.mL21  = glm::vec3(0.0);
//        greenWhiteSH.mL2_1 = glm::vec3(0.0);
//        greenWhiteSH.mL2_2 = glm::vec3(0.0);
//        greenWhiteSH.mL20  = glm::vec3(-1.9816637, -3.96332741, -1.9816637);
//        greenWhiteSH.mL22  = glm::vec3(3.43234229, 6.86468458, 3.43234229);
//
//        SphericalHarmonics redAndWhiteSH;
//        redAndWhiteSH.contribute(glm::vec3(1.0, 0.0, 0.0), glm::vec3(1.0, 0.0, 0.0), 2.0 * M_PI);
//        redAndWhiteSH.contribute(glm::vec3(-1.0, 0.0, 0.0), glm::vec3(1.0, 1.0, 1.0), 2.0 * M_PI);
//
//        SphericalHarmonics blueAndWhiteSH;
//        blueAndWhiteSH.contribute(glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, 0.0, 1.0), 2.0 * M_PI);
//        blueAndWhiteSH.contribute(glm::vec3(-1.0, 0.0, 0.0), glm::vec3(1.0, 1.0, 1.0), 2.0 * M_PI);
//
//        SphericalHarmonics yellowAndWhiteSH;
//        yellowAndWhiteSH.contribute(glm::vec3(1.0, 0.0, 0.0), glm::vec3(1.0, 1.0, 0.0), 2.0 * M_PI);
//        yellowAndWhiteSH.contribute(glm::vec3(-1.0, 0.0, 0.0), glm::vec3(1.0, 1.0, 1.0), 2.0 * M_PI);
//
//        DiffuseLightProbe probe0(bb.center() - glm::vec3(0.5, 0.5, 0.5));
//        probe0.surfelClusterProjectionGroupOffset = 0;
//        probe0.surfelClusterProjectionGroupCount = 1;
//        scene->diffuseLightProbes().push_back(probe0);
//        SurfelClusterProjection projection0;
//        projection0.surfelClusterIndex = 0;
//        projection0.sphericalHarmonics = greenWhiteSH;
//        scene->surfelClusterProjections().push_back(projection0);
//
//        DiffuseLightProbe probe1(bb.center() - glm::vec3(0.5, 0.5, -0.5));
//        probe1.surfelClusterProjectionGroupOffset = 1;
//        probe1.surfelClusterProjectionGroupCount = 1;
//        scene->diffuseLightProbes().push_back(probe1);
//        SurfelClusterProjection projection1;
//        projection1.surfelClusterIndex = 0;
//        projection1.sphericalHarmonics = redAndWhiteSH;
//        scene->surfelClusterProjections().push_back(projection1);
//
//        DiffuseLightProbe probe2(bb.center() - glm::vec3(-0.5, 0.5, 0.5));
//        probe2.surfelClusterProjectionGroupOffset = 2;
//        probe2.surfelClusterProjectionGroupCount = 1;
//        scene->diffuseLightProbes().push_back(probe2);
//        SurfelClusterProjection projection2;
//        projection2.surfelClusterIndex = 0;
//        projection2.sphericalHarmonics = blueAndWhiteSH;
//        scene->surfelClusterProjections().push_back(projection2);
//
//        DiffuseLightProbe probe3(bb.center() - glm::vec3(-0.5, 0.5, -0.5));
//        probe3.surfelClusterProjectionGroupOffset = 3;
//        probe3.surfelClusterProjectionGroupCount = 1;
//        scene->diffuseLightProbes().push_back(probe3);
//        SurfelClusterProjection projection3;
//        projection3.surfelClusterIndex = 0;
//        projection3.sphericalHarmonics = redAndWhiteSH;
//        scene->surfelClusterProjections().push_back(projection3);
//
//        DiffuseLightProbe probe4(bb.center() - glm::vec3(0.5, -0.5, 0.5));
//        probe4.surfelClusterProjectionGroupOffset = 4;
//        probe4.surfelClusterProjectionGroupCount = 1;
//        scene->diffuseLightProbes().push_back(probe4);
//        SurfelClusterProjection projection4;
//        projection4.surfelClusterIndex = 0;
//        projection4.sphericalHarmonics = greenWhiteSH;
//        scene->surfelClusterProjections().push_back(projection4);
//
//        DiffuseLightProbe probe5(bb.center() - glm::vec3(0.5, -0.5, -0.5));
//        probe5.surfelClusterProjectionGroupOffset = 5;
//        probe5.surfelClusterProjectionGroupCount = 1;
//        scene->diffuseLightProbes().push_back(probe5);
//        SurfelClusterProjection projection5;
//        projection5.surfelClusterIndex = 0;
//        projection5.sphericalHarmonics = greenWhiteSH;
//        scene->surfelClusterProjections().push_back(projection5);
//
//        DiffuseLightProbe probe6(bb.center() - glm::vec3(-0.5, -0.5, 0.5));
//        probe6.surfelClusterProjectionGroupOffset = 6;
//        probe6.surfelClusterProjectionGroupCount = 1;
//        scene->diffuseLightProbes().push_back(probe6);
//        SurfelClusterProjection projection6;
//        projection6.surfelClusterIndex = 0;
//        projection6.sphericalHarmonics = greenWhiteSH;
//        scene->surfelClusterProjections().push_back(projection6);
//
//        DiffuseLightProbe probe7(bb.center() - glm::vec3(-0.5, -0.5, -0.5));
//        probe7.surfelClusterProjectionGroupOffset = 7;
//        probe7.surfelClusterProjectionGroupCount = 1;
//        scene->diffuseLightProbes().push_back(probe7);
//        SurfelClusterProjection projection7;
//        projection7.surfelClusterIndex = 0;
//        projection7.sphericalHarmonics = yellowAndWhiteSH;
//        scene->surfelClusterProjections().push_back(projection7);
    }
    
}
