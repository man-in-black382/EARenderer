//
//  Renderer.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 29.03.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "SceneRenderer.hpp"
#include "GLShader.hpp"
#include "ResourcePool.hpp"
#include "Vertex1P4.hpp"
#include "Collision.hpp"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

namespace EARenderer {
    
#pragma mark - Lifecycle
    
    SceneRenderer::SceneRenderer(const Scene* scene, std::shared_ptr<const SurfelData> surfelData, std::shared_ptr<const DiffuseLightProbeData> diffuseProbeData)
    :
    mScene(scene),
    mSurfelData(surfelData),
    mDiffuseProbeData(diffuseProbeData),
    mProbeGridResolution(scene->preferredProbeGridResolution()),

    // Shadow maps
    mShadowMaps(Size2D(1024), mNumberOfCascades),
    mShadowCubeMap(Size2D(1024)),
    mDepthFramebuffer(Size2D(1024)),

    // Image based lighting (IBL)
    mEnvironmentMapCube(Size2D(512)),
    mDiffuseIrradianceMap(Size2D(32)),
    mSpecularIrradianceMap(Size2D(512)),
    mBRDFIntegrationMap(Size2D(512)),
    mIBLFramebuffer(Size2D(512)),

    // Surfels and surfel clusters
    mSurfelsLuminanceMap(surfelData->surfelsGBuffer()->size(), GLTexture::Filter::None),
    mSurfelClustersLuminanceMap(surfelData->surfelClustersGBuffer()->size(), GLTexture::Filter::None),
    mSurfelsLuminanceFramebuffer(mSurfelsLuminanceMap.size()),
    mSurfelClustersLuminanceFramebuffer(mSurfelClustersLuminanceMap.size()),

    // Diffuse light probes
    mGridProbesSHMaps {
        GLHDRTexture3D(Size2D(mProbeGridResolution.x, mProbeGridResolution.y), mProbeGridResolution.z),
        GLHDRTexture3D(Size2D(mProbeGridResolution.x, mProbeGridResolution.y), mProbeGridResolution.z),
        GLHDRTexture3D(Size2D(mProbeGridResolution.x, mProbeGridResolution.y), mProbeGridResolution.z),
        GLHDRTexture3D(Size2D(mProbeGridResolution.x, mProbeGridResolution.y), mProbeGridResolution.z),
        GLHDRTexture3D(Size2D(mProbeGridResolution.x, mProbeGridResolution.y), mProbeGridResolution.z),
        GLHDRTexture3D(Size2D(mProbeGridResolution.x, mProbeGridResolution.y), mProbeGridResolution.z),
        GLHDRTexture3D(Size2D(mProbeGridResolution.x, mProbeGridResolution.y), mProbeGridResolution.z)
    },
    mGridProbesSHFramebuffer(Size2D(mProbeGridResolution.x, mProbeGridResolution.y))
    {
        mDiffuseProbesVAO.initialize(diffuseProbeData->probes(), {
            GLVertexAttribute::UniqueAttribute(sizeof(glm::vec3), glm::vec3::length()),
            GLVertexAttribute::UniqueAttribute(sizeof(glm::vec3), glm::vec3::length()),
            GLVertexAttribute::UniqueAttribute(sizeof(glm::vec2), glm::vec2::length())
        });

        setupGLState();
        setupTextures();
        setupFramebuffers();
        
//        mIBLFramebuffer.attachColorTextures({ mSurfelsLuminanceMap, mSurfelClustersLuminanceMap });

//        convertEquirectangularMapToCubemap();
//        buildDiffuseIrradianceMap();
//        buildSpecularIrradianceMap();
//        buildBRDFIntegrationMap();
    }
    
#pragma mark - Setters
    
    void SceneRenderer::setDefaultRenderComponentsProvider(DefaultRenderComponentsProviding *provider) {
        mDefaultRenderComponentsProvider = provider;
    }

#pragma mark - Math
    
    bool SceneRenderer::raySelectsMesh(const Ray3D& ray, ID& meshID) {
        float minimumDistance = std::numeric_limits<float>::max();
        ID closestMeshID = IDNotFound;
        
        for (ID id : mScene->meshInstances()) {
            const MeshInstance& meshInstance = mScene->meshInstances()[id];
            glm::mat4 modelMatrix = meshInstance.transformation().modelMatrix();
            Ray3D meshLocalSpaceRay = ray;//.transformedBy(glm::inverse(modelMatrix)); // Had to comment the transformation and I don't remember why :(
            
            float distance = 0;
            if (Collision::RayAABB(meshLocalSpaceRay, meshInstance.boundingBox(), distance)) {
                // Intersection distance is in the mesh's local space
                // Scale local space ray's direction vector (which is a unit vector) accordingly
                glm::vec3 localScaledDirection = meshLocalSpaceRay.direction * distance;
                // Transform back to world space to obtain real origin -> intersection distance
                glm::vec3 worldScaledDirection = modelMatrix * glm::vec4(localScaledDirection, 1.0);
                float worldDistance = glm::length(worldScaledDirection);
                
                if (worldDistance < minimumDistance) {
                    minimumDistance = worldDistance;
                    closestMeshID = id;
                }
            }
        }
        
        meshID = closestMeshID;
        return closestMeshID != IDNotFound;
    }

#pragma mark - Initial setup

    void SceneRenderer::setupGLState() {
        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glClearColor(0.0, 0.0, 0.0, 1.0);
        glClearDepth(1.0);
        glDepthFunc(GL_LEQUAL);
    }

    void SceneRenderer::setupTextures() {
        mSpecularIrradianceMap.generateMipmaps();
    }

    void SceneRenderer::setupFramebuffers() {
        mSurfelsLuminanceFramebuffer.attachTexture(mSurfelsLuminanceMap);
        mSurfelClustersLuminanceFramebuffer.attachTexture(mSurfelClustersLuminanceMap);

        mGridProbesSHFramebuffer.attachTexture(mGridProbesSHMaps[0], GLFramebuffer::ColorAttachment::Attachment0);
        mGridProbesSHFramebuffer.attachTexture(mGridProbesSHMaps[1], GLFramebuffer::ColorAttachment::Attachment1);
        mGridProbesSHFramebuffer.attachTexture(mGridProbesSHMaps[2], GLFramebuffer::ColorAttachment::Attachment2);
        mGridProbesSHFramebuffer.attachTexture(mGridProbesSHMaps[3], GLFramebuffer::ColorAttachment::Attachment3);
        mGridProbesSHFramebuffer.attachTexture(mGridProbesSHMaps[4], GLFramebuffer::ColorAttachment::Attachment4);
        mGridProbesSHFramebuffer.attachTexture(mGridProbesSHMaps[5], GLFramebuffer::ColorAttachment::Attachment5);
        mGridProbesSHFramebuffer.attachTexture(mGridProbesSHMaps[6], GLFramebuffer::ColorAttachment::Attachment6);
    }

#pragma mark - Rendering
#pragma mark - Baking
    
    void SceneRenderer::convertEquirectangularMapToCubemap() {
        mIBLFramebuffer.bind();
        mIBLFramebuffer.attachTexture(mEnvironmentMapCube);
        mIBLFramebuffer.viewport().apply();
        
        mEqurectangularMapConversionShader.bind();
        mEqurectangularMapConversionShader.ensureSamplerValidity([this]() {
            mEqurectangularMapConversionShader.setEquirectangularEnvironmentMap(mScene->skybox()->equirectangularMap());
        });
        
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDrawArrays(GL_TRIANGLES, 0, 4);
        
        mEnvironmentMapCube.generateMipmaps();
    }
    
    void SceneRenderer::buildDiffuseIrradianceMap() {
        mIBLFramebuffer.bind();
        mIBLFramebuffer.attachTexture(mDiffuseIrradianceMap);
        GLViewport(mDiffuseIrradianceMap.size()).apply();
        
        mRadianceConvolutionShader.bind();
        mRadianceConvolutionShader.setAlgorithm(GLSLRadianceConvolution::Algorithm::diffuse);
        mRadianceConvolutionShader.ensureSamplerValidity([this]() {
            mRadianceConvolutionShader.setEnvironmentRadianceMap(mEnvironmentMapCube);
        });
        
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDrawArrays(GL_TRIANGLES, 0, 4);
    }
    
    void SceneRenderer::buildSpecularIrradianceMap() {
        mIBLFramebuffer.bind();
        GLViewport(mSpecularIrradianceMap.size()).apply();
        
        mRadianceConvolutionShader.bind();
        mRadianceConvolutionShader.setAlgorithm(GLSLRadianceConvolution::Algorithm::specular);
        mRadianceConvolutionShader.ensureSamplerValidity([this]() {
            mRadianceConvolutionShader.setEnvironmentRadianceMap(mEnvironmentMapCube);
        });
        
        for (int16_t mipLevel = 0; mipLevel < mNumberOfIrradianceMips; ++mipLevel) {
            Size2D mipSize = mEnvironmentMapCube.size().transformedBy(glm::vec2(std::pow(0.5, mipLevel)));
            GLViewport(mipSize).apply();
            
            float roughness = (float)mipLevel / (float)(mNumberOfIrradianceMips - 1);
            mRadianceConvolutionShader.setRoughness(roughness);
            
            mIBLFramebuffer.attachTexture(mSpecularIrradianceMap, GLFramebuffer::ColorAttachment::Attachment0, mipLevel);
            
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glDrawArrays(GL_TRIANGLES, 0, 4);
        }
    }
    
    void SceneRenderer::buildBRDFIntegrationMap() {
        mIBLFramebuffer.bind();
        mIBLFramebuffer.attachTexture(mBRDFIntegrationMap);
        mIBLFramebuffer.viewport().apply();
        
        mBRDFIntegrationShader.bind();
        
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDrawArrays(GL_TRIANGLES, 0, 4);
    }

#pragma mark - Runtime

    void SceneRenderer::bindDefaultFramebuffer() {
        if (mDefaultRenderComponentsProvider) {
            mDefaultRenderComponentsProvider->bindSystemFramebuffer();
            mDefaultRenderComponentsProvider->defaultViewport().apply();
        }
    }

    void SceneRenderer::renderShadowMapsForDirectionalLights() {
        // Fill shadow map
        mDirectionalDepthShader.bind();
        mDepthFramebuffer.bind();
        mDepthFramebuffer.viewport().apply();

        for (uint8_t cascade = 0; cascade < mShadowCascades.amount; cascade++) {
            mDepthFramebuffer.attachTextureLayer(mShadowMaps, cascade);
            glClear(GL_DEPTH_BUFFER_BIT);

            for (ID meshInstanceID : mScene->meshInstances()) {
                auto& instance = mScene->meshInstances()[meshInstanceID];
                auto& subMeshes = ResourcePool::shared().meshes[instance.meshID()].subMeshes();

                mDirectionalDepthShader.setModelMatrix(instance.transformation().modelMatrix());
                mDirectionalDepthShader.setViewProjectionMatrix(mShadowCascades.lightViewProjections[cascade]);

                for (ID subMeshID : subMeshes) {
                    auto& subMesh = subMeshes[subMeshID];
                    subMesh.draw();
                }
            }
        }
    }

    void SceneRenderer::relightSurfels() {
        const DirectionalLight& directionalLight = mScene->directionalLight();

        mSurfelsLuminanceFramebuffer.bind();
        mSurfelsLuminanceFramebuffer.viewport().apply();

        mSurfelLightingShader.bind();
        mSurfelLightingShader.setLight(directionalLight);
        mSurfelLightingShader.ensureSamplerValidity([&]() {
            mSurfelLightingShader.setShadowMapsUniforms(mShadowCascades, mShadowMaps);
            mSurfelLightingShader.setSurfelsGBuffer(*mSurfelData->surfelsGBuffer());
        });

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }

    void SceneRenderer::averageSurfelClusterLuminances() {
        mSurfelClusterAveragingShader.bind();
        mSurfelClustersLuminanceFramebuffer.bind();
        mSurfelClustersLuminanceFramebuffer.viewport().apply();

        mSurfelClusterAveragingShader.ensureSamplerValidity([&]() {
            mSurfelClusterAveragingShader.setSurfelClustersGBuffer(*mSurfelData->surfelClustersGBuffer());
            mSurfelClusterAveragingShader.setSurfelsLuminaceMap(mSurfelsLuminanceMap);
        });

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }

    void SceneRenderer::updateGridProbes() {
        // Disable blending because this is spherical harmonics, not colors!
        // Nothing to blend here
        glDisable(GL_BLEND);

        mGridProbesUpdateShader.bind();
        mGridProbesUpdateShader.ensureSamplerValidity([&] {
            mGridProbesUpdateShader.setSurfelClustersLuminaceMap(mSurfelClustersLuminanceMap);
            mGridProbesUpdateShader.setProbeProjectionsMetadata(*mDiffuseProbeData->probeClusterProjectionsMetadataBufferTexture());
            mGridProbesUpdateShader.setProjectionClusterIndices(*mDiffuseProbeData->projectionClusterIndicesBufferTexture());
            mGridProbesUpdateShader.setProjectionClusterSphericalHarmonics(*mDiffuseProbeData->projectionClusterSHsBufferTexture());
            mGridProbesUpdateShader.setProbesGridResolution(mProbeGridResolution);
        });

        mGridProbesSHFramebuffer.bind();
        mGridProbesSHFramebuffer.viewport().apply();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, (GLsizei)mProbeGridResolution.z);

        glEnable(GL_BLEND);
    }

#pragma mark - Public interface

    void SceneRenderer::prepareFrame() {
        const DirectionalLight& directionalLight = mScene->directionalLight();
        mShadowCascades = directionalLight.cascadesForWorldBoundingBox(mScene->boundingBox());

        renderShadowMapsForDirectionalLights();
        relightSurfels();
        averageSurfelClusterLuminances();
        updateGridProbes();

        bindDefaultFramebuffer();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void SceneRenderer::renderMeshes() {
        const DirectionalLight& directionalLight = mScene->directionalLight();

        bindDefaultFramebuffer();
        
        mCookTorranceShader.bind();
        mCookTorranceShader.ensureSamplerValidity([&]() {
            mCookTorranceShader.setCamera(*(mScene->camera()));
            mCookTorranceShader.setLight(directionalLight);
            mCookTorranceShader.setShadowMapsUniforms(mShadowCascades, mShadowMaps);
            mCookTorranceShader.setWorldBoundingBox(mScene->lightBakingVolume());
            mCookTorranceShader.setGridProbesSHTextures(mGridProbesSHMaps);
            mCookTorranceShader.setDiffuseProbeOcclusionMapsAtlas(*mDiffuseProbeData->occlusionMapAtlas());
            mCookTorranceShader.setCubemapTexCoordsMap(*mDiffuseProbeData->cubeFaceTextureCoordsMap());
//            mCookTorranceShader.setIBLUniforms(mDiffuseIrradianceMap, mSpecularIrradianceMap, mBRDFIntegrationMap, mNumberOfIrradianceMips);
        });

        for (ID instanceID : mScene->meshInstances()) {
            auto& instance = mScene->meshInstances()[instanceID];
            auto& subMeshes = ResourcePool::shared().meshes[instance.meshID()].subMeshes();

            mCookTorranceShader.setModelMatrix(instance.transformation().modelMatrix());

            for (ID subMeshID : subMeshes) {
                auto& subMesh = subMeshes[subMeshID];
                auto& material = ResourcePool::shared().materials[instance.materialIDForSubMeshID(subMeshID)];

                mCookTorranceShader.ensureSamplerValidity([this, &material]() {
                    mCookTorranceShader.setMaterial(material);
                });

                subMesh.draw();
            }
        }

//        glDisable(GL_DEPTH_TEST);
//
//        mFSQuadShader.bind();
//        mFSQuadShader.setApplyToneMapping(false);
//
//        Rect2D viewportRect(mDiffuseProbeData->occlusionMapAtlas()->size());
//        GLViewport(viewportRect).apply();
//
//        mFSQuadShader.ensureSamplerValidity([this]() {
//            mFSQuadShader.setTexture(*mDiffuseProbeData->occlusionMapAtlas());
//        });
//
//        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
//        glEnable(GL_DEPTH_TEST);
    }

    void SceneRenderer::renderSkybox() {
        bindDefaultFramebuffer();
        mSkyboxShader.bind();
        mSkyboxShader.ensureSamplerValidity([this]() {
            mSkyboxShader.setViewMatrix(mScene->camera()->viewMatrix());
            mSkyboxShader.setProjectionMatrix(mScene->camera()->projectionMatrix());
            mSkyboxShader.setEquirectangularMap(mScene->skybox()->equirectangularMap());
        });
        mScene->skybox()->draw();
    }

    void SceneRenderer::renderSurfelsGBuffer() {
        bindDefaultFramebuffer();
        glDisable(GL_DEPTH_TEST);

        mFSQuadShader.bind();
        mFSQuadShader.setApplyToneMapping(false);

        float defaultViewportWidth = mDefaultRenderComponentsProvider->defaultViewport().frame().size.width;
        Rect2D viewportRect(Size2D(400, 400));

        for (size_t i = 0; i < mSurfelData->surfelsGBuffer()->layers(); i++) {
            GLViewport(viewportRect).apply();

            mFSQuadShader.ensureSamplerValidity([this, i]() {
                mFSQuadShader.setTexture(*mSurfelData->surfelsGBuffer(), i);
            });

            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

            if ((viewportRect.origin.x + 400) > defaultViewportWidth) {
                viewportRect.origin.x = 0.0;
                viewportRect.origin.y += 400;
            } else {
                viewportRect.origin.x += 400.0;
            }
        }

        glEnable(GL_DEPTH_TEST);
    }

    void SceneRenderer::renderSurfelLuminances() {
        bindDefaultFramebuffer();
        glDisable(GL_DEPTH_TEST);

        mFSQuadShader.bind();
        mFSQuadShader.setApplyToneMapping(true);

        Rect2D viewportRect(Size2D(100, 100));
        GLViewport(viewportRect).apply();

        mFSQuadShader.ensureSamplerValidity([this]() {
            mFSQuadShader.setTexture(mSurfelsLuminanceMap);
        });

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glEnable(GL_DEPTH_TEST);
    }

    void SceneRenderer::renderSurfelClusterLuminances() {
        bindDefaultFramebuffer();
        glDisable(GL_DEPTH_TEST);

        mFSQuadShader.bind();
        mFSQuadShader.setApplyToneMapping(true);

        Rect2D viewportRect({ 100, 0 }, { 100, 100 });
        GLViewport(viewportRect).apply();

        mFSQuadShader.ensureSamplerValidity([this]() {
            mFSQuadShader.setTexture(mSurfelClustersLuminanceMap);
        });

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glEnable(GL_DEPTH_TEST);
    }

    void SceneRenderer::renderDiffuseGridProbes(float radius) {
        bindDefaultFramebuffer();
        mDiffuseProbesVAO.bind();
        mGridProbeRenderingShader.bind();
        mGridProbeRenderingShader.setCamera(*mScene->camera());
        mGridProbeRenderingShader.setSphereRadius(radius);
        mGridProbeRenderingShader.setWorldBoundingBox(mScene->lightBakingVolume());
        mGridProbeRenderingShader.setProbesGridResolution(mProbeGridResolution);
        mGridProbeRenderingShader.ensureSamplerValidity([&] {
            mGridProbeRenderingShader.setGridProbesSHTextures(mGridProbesSHMaps);
        });

        glDrawArrays(GL_POINTS, 0, (GLsizei)mDiffuseProbeData->probes().size());
    }

    void SceneRenderer::renderLinksForDiffuseProbe(size_t probeIndex) {
        bindDefaultFramebuffer();
        mDiffuseProbesVAO.bind();
        mLightProbeLinksRenderingShader.bind();
        mLightProbeLinksRenderingShader.setCamera(*mScene->camera());
        mLightProbeLinksRenderingShader.setWorldBoundingBox(mScene->lightBakingVolume());
        mLightProbeLinksRenderingShader.ensureSamplerValidity([&] {
            mLightProbeLinksRenderingShader.setProbeProjectionsMetadata(*mDiffuseProbeData->probeClusterProjectionsMetadataBufferTexture());
            mLightProbeLinksRenderingShader.setProjectionClusterIndices(*mDiffuseProbeData->projectionClusterIndicesBufferTexture());
            mLightProbeLinksRenderingShader.setSurfelClusterCenters(*mSurfelData->surfelClusterCentersBufferTexture());
        });

        glDrawArrays(GL_POINTS, (GLint)probeIndex, 1);
    }

}
