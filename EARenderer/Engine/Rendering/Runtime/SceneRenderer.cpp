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
    
    SceneRenderer::SceneRenderer(Scene* scene)
    :
    mScene(scene),
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
    mSurfelsGBuffer(surfelsGBufferData()),
    mSurfelClustersGBuffer(surfelClustersGBufferData()),
    mSurfelsLuminanceMap(mSurfelsGBuffer.size(), GLTexture::Filter::None),
    mSurfelClustersLuminanceMap(mSurfelClustersGBuffer.size(), GLTexture::Filter::None),
    mSurfelsLuminanceFramebuffer(mSurfelsGBuffer.size()),
    mSurfelClustersLuminanceFramebuffer(mSurfelClustersGBuffer.size()),

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

    mLightmapProbeIndicesMap(scene->diffuseProbeLightmapIndices()),
    mLightmapProbesSHMaps(scene->preferredProbeLightmapResolution(), 7),
    mGridProbesSHFramebuffer(Size2D(mProbeGridResolution.x, mProbeGridResolution.y)),
    mLightmapProbesSHFramebuffer(mLightmapProbesSHMaps.size())
    {
        mDiffuseProbesVAO.initialize(scene->diffuseLightProbes(), {
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
            MeshInstance& meshInstance = mScene->meshInstances()[id];
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
        mProjectionClusterSHsBufferTexture.buffer().initialize(surfelProjectionsSH());
        mProjectionClusterIndicesBufferTexture.buffer().initialize(surfelClusterIndices());
        mDiffuseProbeClusterProjectionsBufferTexture.buffer().initialize(probeProjectionsMetadata());
        mSurfelClusterCentersBufferTexture.buffer().initialize(surfelClusterCenters());

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

        mLightmapProbesSHFramebuffer.attachTextureLayer(mLightmapProbesSHMaps, 0, GLFramebuffer::ColorAttachment::Attachment0);
        mLightmapProbesSHFramebuffer.attachTextureLayer(mLightmapProbesSHMaps, 1, GLFramebuffer::ColorAttachment::Attachment1);
        mLightmapProbesSHFramebuffer.attachTextureLayer(mLightmapProbesSHMaps, 2, GLFramebuffer::ColorAttachment::Attachment2);
        mLightmapProbesSHFramebuffer.attachTextureLayer(mLightmapProbesSHMaps, 3, GLFramebuffer::ColorAttachment::Attachment3);
        mLightmapProbesSHFramebuffer.attachTextureLayer(mLightmapProbesSHMaps, 4, GLFramebuffer::ColorAttachment::Attachment4);
        mLightmapProbesSHFramebuffer.attachTextureLayer(mLightmapProbesSHMaps, 5, GLFramebuffer::ColorAttachment::Attachment5);
        mLightmapProbesSHFramebuffer.attachTextureLayer(mLightmapProbesSHMaps, 6, GLFramebuffer::ColorAttachment::Attachment6);
    }

#pragma mark - Data preparation

    std::vector<std::vector<glm::vec3>> SceneRenderer::surfelsGBufferData() const {
        std::vector<std::vector<glm::vec3>> bufferData;
        bufferData.emplace_back();
        bufferData.emplace_back();
        bufferData.emplace_back();
        bufferData.emplace_back();

        for (auto& surfel : mScene->surfels()) {
            bufferData[0].emplace_back(surfel.position);
            bufferData[1].emplace_back(surfel.normal);
            bufferData[2].emplace_back(surfel.albedo);
            bufferData[3].emplace_back(surfel.lightmapUV.x, surfel.lightmapUV.y, 0.0);
        }

        return bufferData;
    }

    std::vector<uint8_t> SceneRenderer::surfelClustersGBufferData() const {
        std::vector<uint8_t> data;

        // Pack surfel offset's 24 LS bits into 3 consequtive ubyte values
        // Then pack the surfel count into 1 ubyte that follows 3 offset bytes
        // Surfel generator cannot generate more than 256 surfels per cluster by design
        // so 1 byte per surfel count will be enough
        // Fragment shader will then unpack these values from RGB and Alpha channels respectively
        for (auto& cluster : mScene->surfelClusters()) {
            uint8_t b = cluster.surfelOffset & 0xFF;
            uint8_t g = (cluster.surfelOffset >> 8) & 0xFF;
            uint8_t r = (cluster.surfelOffset >> 16) & 0xFF;
            uint8_t a = cluster.surfelCount;
            data.push_back(r);
            data.push_back(g);
            data.push_back(b);
            data.push_back(a);
        }
        return data;
    }

    std::vector<glm::vec3> SceneRenderer::surfelClusterCenters() const {
        std::vector<glm::vec3> centers;
        for (auto& cluster : mScene->surfelClusters()) {
            centers.push_back(cluster.center);
        }
        return centers;
    }

    std::vector<SphericalHarmonics> SceneRenderer::surfelProjectionsSH() const {
        std::vector<SphericalHarmonics> shs;
        for (auto& projection : mScene->surfelClusterProjections()) {
            shs.push_back(projection.sphericalHarmonics);
        }
        return shs;
    }

    std::vector<uint32_t> SceneRenderer::surfelClusterIndices() const {
        std::vector<uint32_t> indices;
        for (auto& projection : mScene->surfelClusterProjections()) {
            indices.push_back(static_cast<uint32_t>(projection.surfelClusterIndex));
        }
        return indices;
    }

    std::vector<uint32_t> SceneRenderer::probeProjectionsMetadata() const {
        std::vector<uint32_t> metadata;
        for (auto& probe : mScene->diffuseLightProbes()) {
            metadata.push_back((uint32_t)probe.surfelClusterProjectionGroupOffset);
            metadata.push_back((uint32_t)probe.surfelClusterProjectionGroupCount);
        }
        return metadata;
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
        DirectionalLight& directionalLight = mScene->directionalLight();

        mSurfelsLuminanceFramebuffer.bind();
        mSurfelsLuminanceFramebuffer.viewport().apply();

        mSurfelLightingShader.bind();
        mSurfelLightingShader.setLight(directionalLight);
        mSurfelLightingShader.ensureSamplerValidity([&]() {
            mSurfelLightingShader.setShadowMapsUniforms(mShadowCascades, mShadowMaps);
            mSurfelLightingShader.setSurfelsGBuffer(mSurfelsGBuffer);
        });

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }

    void SceneRenderer::averageSurfelClusterLuminances() {
        mSurfelClusterAveragingShader.bind();
        mSurfelClustersLuminanceFramebuffer.bind();
        mSurfelClustersLuminanceFramebuffer.viewport().apply();

        mSurfelClusterAveragingShader.ensureSamplerValidity([&]() {
            mSurfelClusterAveragingShader.setSurfelClustersGBuffer(mSurfelClustersGBuffer);
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
            mGridProbesUpdateShader.setProbeProjectionsMetadata(mDiffuseProbeClusterProjectionsBufferTexture);
            mGridProbesUpdateShader.setProjectionClusterIndices(mProjectionClusterIndicesBufferTexture);
            mGridProbesUpdateShader.setProjectionClusterSphericalHarmonics(mProjectionClusterSHsBufferTexture);
            mGridProbesUpdateShader.setProbesGridResolution(mProbeGridResolution);
        });

        mGridProbesSHFramebuffer.bind();
        mGridProbesSHFramebuffer.viewport().apply();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, (GLsizei)mProbeGridResolution.z);

        glEnable(GL_BLEND);
    }

    void SceneRenderer::updateLightmapProbes() {
        // Disable blending because this is spherical harmonics, not colors!
        // Nothing to blend here
        glDisable(GL_BLEND);

        mLightmapProbesSHFramebuffer.bind();
        mLightmapProbesSHFramebuffer.viewport().apply();

        mLightmapProbesUpdateShader.bind();
        mLightmapProbesUpdateShader.ensureSamplerValidity([&] {
            mLightmapProbesUpdateShader.setLightmapProbeIndicesMap(mLightmapProbeIndicesMap);
            mLightmapProbesUpdateShader.setSurfelClustersLuminaceMap(mSurfelClustersLuminanceMap);
            mLightmapProbesUpdateShader.setProbeProjectionsMetadata(mDiffuseProbeClusterProjectionsBufferTexture);
            mLightmapProbesUpdateShader.setProjectionClusterIndices(mProjectionClusterIndicesBufferTexture);
            mLightmapProbesUpdateShader.setProjectionClusterSphericalHarmonics(mProjectionClusterSHsBufferTexture);
        });

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        glEnable(GL_BLEND);
    }

#pragma mark - Public interface

    void SceneRenderer::prepareFrame() {
        DirectionalLight& directionalLight = mScene->directionalLight();
        mShadowCascades = directionalLight.cascadesForWorldBoundingBox(mScene->boundingBox());

        renderShadowMapsForDirectionalLights();
        relightSurfels();
        averageSurfelClusterLuminances();
//        updateGridProbes();
//        updateLightmapProbes();

        bindDefaultFramebuffer();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void SceneRenderer::renderMeshes() {
        DirectionalLight& directionalLight = mScene->directionalLight();

        bindDefaultFramebuffer();
        
        mCookTorranceShader.bind();
        mCookTorranceShader.ensureSamplerValidity([&]() {
            mCookTorranceShader.setCamera(*(mScene->camera()));
            mCookTorranceShader.setLight(directionalLight);
            mCookTorranceShader.setShadowMapsUniforms(mShadowCascades, mShadowMaps);
            mCookTorranceShader.setWorldBoundingBox(mScene->lightBakingVolume());
            mCookTorranceShader.setGridProbesSHTextures(mGridProbesSHMaps);
            mCookTorranceShader.setLightmapProbesSHTextures(mLightmapProbesSHMaps);
            mCookTorranceShader.setProbesGridResolution(mProbeGridResolution);
//            mCookTorranceShader.setIBLUniforms(mDiffuseIrradianceMap, mSpecularIrradianceMap, mBRDFIntegrationMap, mNumberOfIrradianceMips);
        });

        mCookTorranceShader.setGeometryType(GLSLCookTorrance::GeometryType::Static);

        for (ID staticMeshInstanceID : mScene->staticMeshInstanceIDs()) {
            auto& instance = mScene->meshInstances()[staticMeshInstanceID];
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

        mCookTorranceShader.setGeometryType(GLSLCookTorrance::GeometryType::Dynamic);

        for (ID dynamicMeshInstanceID : mScene->dynamicMeshInstanceIDs()) {
            auto& instance = mScene->meshInstances()[dynamicMeshInstanceID];
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
//        Rect2D viewportRect(Size2D(128));
//        GLViewport(viewportRect).apply();
//
//        mFSQuadShader.ensureSamplerValidity([this]() {
//            mFSQuadShader.setTexture(mLightmapProbeIndicesMap);
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

        for (size_t i = 0; i < mSurfelsGBuffer.layers(); i++) {
            GLViewport(viewportRect).apply();

            mFSQuadShader.ensureSamplerValidity([this, i]() {
                mFSQuadShader.setTexture(mSurfelsGBuffer, i);
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

        glDrawArrays(GL_POINTS, 0, (GLsizei)mScene->diffuseLightProbes().size());
    }

    void SceneRenderer::renderDiffuseLightmapProbes(float radius) {
        bindDefaultFramebuffer();
        mDiffuseProbesVAO.bind();
        mLightmapProbeRenderingShader.bind();
        mLightmapProbeRenderingShader.setCamera(*mScene->camera());
        mLightmapProbeRenderingShader.setSphereRadius(radius);
        mLightmapProbeRenderingShader.ensureSamplerValidity([&] {
            mLightmapProbeRenderingShader.setLightmapProbesSHTextures(mLightmapProbesSHMaps);
        });

        glDrawArrays(GL_POINTS, 0, (GLsizei)mScene->diffuseLightProbes().size());
    }

    void SceneRenderer::renderLinksForDiffuseProbe(size_t probeIndex) {
        bindDefaultFramebuffer();
        mDiffuseProbesVAO.bind();
        mLightProbeLinksRenderingShader.bind();
        mLightProbeLinksRenderingShader.setCamera(*mScene->camera());
        mLightProbeLinksRenderingShader.setWorldBoundingBox(mScene->lightBakingVolume());
        mLightProbeLinksRenderingShader.ensureSamplerValidity([&] {
            mLightProbeLinksRenderingShader.setProbeProjectionsMetadata(mDiffuseProbeClusterProjectionsBufferTexture);
            mLightProbeLinksRenderingShader.setProjectionClusterIndices(mProjectionClusterIndicesBufferTexture);
            mLightProbeLinksRenderingShader.setSurfelClusterCenters(mSurfelClusterCentersBufferTexture);
        });

        glDrawArrays(GL_POINTS, (GLint)probeIndex, 1);
    }

}
