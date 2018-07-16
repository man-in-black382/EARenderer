//
//  DeferredSceneRenderer.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 30.06.2018.
//  Copyright © 2018 MPO. All rights reserved.
//

#include "DeferredSceneRenderer.hpp"
#include "GLShader.hpp"
#include "ResourcePool.hpp"
#include "Vertex1P4.hpp"
#include "Collision.hpp"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

namespace EARenderer {

#pragma mark - Lifecycle

    DeferredSceneRenderer::DeferredSceneRenderer(const Scene* scene,
                                                 const DefaultRenderComponentsProviding *provider,
                                                 const RenderingSettings& settings,
                                                 std::shared_ptr<const SurfelData> surfelData,
                                                 std::shared_ptr<const DiffuseLightProbeData> diffuseProbeData,
                                                 std::shared_ptr<const SceneGBuffer> GBuffer)
    :
    mScene(scene),
    mDefaultRenderComponentsProvider(provider),
    mSettings(settings),
    mSurfelData(surfelData),
    mDiffuseProbeData(diffuseProbeData),
    mGBuffer(GBuffer),
    mProbeGridResolution(scene->preferredProbeGridResolution()),

    // Shadow maps
    mDirectionalShadowTexturePool(std::make_shared<PostprocessTexturePool>(Size2D(1500))),
    mDirectionalExponentialShadowMap(mDirectionalShadowTexturePool->claim()),

    // Surfels and surfel clusters
    mSurfelsLuminanceMap(surfelData->surfelsGBuffer()->size(), GLTexture::Filter::None),
    mSurfelClustersLuminanceMap(surfelData->surfelClustersGBuffer()->size(), GLTexture::Filter::None),
    mSurfelsLuminanceFramebuffer(mSurfelsLuminanceMap.size()),
    mSurfelClustersLuminanceFramebuffer(mSurfelClustersLuminanceMap.size()),

    // Diffuse light probes
    mGridProbesSHMaps {
        GLLDRTexture3D(Size2D(mProbeGridResolution.x, mProbeGridResolution.y), mProbeGridResolution.z),
        GLLDRTexture3D(Size2D(mProbeGridResolution.x, mProbeGridResolution.y), mProbeGridResolution.z),
        GLLDRTexture3D(Size2D(mProbeGridResolution.x, mProbeGridResolution.y), mProbeGridResolution.z),
        GLLDRTexture3D(Size2D(mProbeGridResolution.x, mProbeGridResolution.y), mProbeGridResolution.z)
    },
    mGridProbesSHFramebuffer(Size2D(mProbeGridResolution.x, mProbeGridResolution.y)),

    // Effects
    mPostprocessTexturePool(std::make_shared<PostprocessTexturePool>(settings.resolution)),

    // Output frame
    mFrame(mPostprocessTexturePool->claim()),
    mPreviousFrame(mPostprocessTexturePool->claim()),
    mThresholdFilteredOutputFrame(mPostprocessTexturePool->claim())//,
//    mOutputDepthRenderbuffer(mOutputFrame->size()),
//    mOutputFramebuffer(mOutputFrame->size())
    {
        setupGLState();
        setupFramebuffers();
    }

#pragma mark - Setters

    void DeferredSceneRenderer::setRenderingSettings(const RenderingSettings& settings) {
        mSettings = settings;
    }

#pragma mark - Getters

    const FrustumCascades& DeferredSceneRenderer::shadowCascades() const {
        return mShadowCascades;
    }

#pragma mark - Initial setup

    void DeferredSceneRenderer::setupGLState() {
        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
        glDisable(GL_BLEND);
        glClearColor(0.0, 0.0, 0.0, 1.0);
        glClearDepth(1.0);
        glDepthFunc(GL_LEQUAL);
    }

    void DeferredSceneRenderer::setupFramebuffers() {
        mSurfelsLuminanceFramebuffer.attachTexture(mSurfelsLuminanceMap);
        mSurfelClustersLuminanceFramebuffer.attachTexture(mSurfelClustersLuminanceMap);

        mGridProbesSHFramebuffer.attachTexture(mGridProbesSHMaps[0], GLFramebuffer::ColorAttachment::Attachment0);
        mGridProbesSHFramebuffer.attachTexture(mGridProbesSHMaps[1], GLFramebuffer::ColorAttachment::Attachment1);
        mGridProbesSHFramebuffer.attachTexture(mGridProbesSHMaps[2], GLFramebuffer::ColorAttachment::Attachment2);
        mGridProbesSHFramebuffer.attachTexture(mGridProbesSHMaps[3], GLFramebuffer::ColorAttachment::Attachment3);
    }

#pragma mark - Rendering
#pragma mark - Runtime

    void DeferredSceneRenderer::bindDefaultFramebuffer() {
        if (mDefaultRenderComponentsProvider) {
            mDefaultRenderComponentsProvider->bindSystemFramebuffer();
            mDefaultRenderComponentsProvider->defaultViewport().apply();
        }
    }

    void DeferredSceneRenderer::swapFrames() {
        std::swap(mFrame, mPreviousFrame);
    }

    void DeferredSceneRenderer::performDepthPrepass() {
        bindDefaultFramebuffer();
        glClear(GL_DEPTH_BUFFER_BIT);

        mDepthPrepassShader.bind();
        mDepthPrepassShader.setCamera(*mScene->camera());

        for (ID instanceID : mScene->meshInstances()) {
            auto& instance = mScene->meshInstances()[instanceID];
            auto& subMeshes = ResourcePool::shared().meshes[instance.meshID()].subMeshes();

            mDepthPrepassShader.setModelMatrix(instance.transformation().modelMatrix());

            for (ID subMeshID : subMeshes) {
                auto& subMesh = subMeshes[subMeshID];
                subMesh.draw();
            }
        }
    }

    void DeferredSceneRenderer::renderExponentialShadowMapsForDirectionalLight() {
        auto renderTarget = mDirectionalShadowTexturePool->claim();
        mDirectionalShadowTexturePool->redirectRenderingToTextures(renderTarget);

        mDirectionalESMShader.bind();
        mDirectionalESMShader.setESMFactor(mSettings.meshSettings.ESMFactor);

        for (size_t cascade = 0; cascade < mShadowCascades.amount; cascade++) {
            // Ensure only one texture channel will be written to for each respective cascade
            glColorMask(cascade == 0, cascade == 1, cascade == 2, cascade == 3);

            mDirectionalESMShader.setLightMatrix(mShadowCascades.lightViewProjections[cascade]);

            for (ID meshInstanceID : mScene->meshInstances()) {
                auto& instance = mScene->meshInstances()[meshInstanceID];
                auto& subMeshes = ResourcePool::shared().meshes[instance.meshID()].subMeshes();

                mDirectionalESMShader.setModelMatrix(instance.transformation().modelMatrix());

                for (ID subMeshID : subMeshes) {
                    auto& subMesh = subMeshes[subMeshID];
                    subMesh.draw();
                }
            }
        }

        glColorMask(true, true, true, true);

        mShadowBlurEffect.blur(renderTarget, mDirectionalExponentialShadowMap,
                               mDirectionalShadowTexturePool, mSettings.meshSettings.shadowBlur);

        mDirectionalShadowTexturePool->putBack(renderTarget);
    }

    void DeferredSceneRenderer::relightSurfels() {
        const DirectionalLight& directionalLight = mScene->directionalLight();

        mSurfelsLuminanceFramebuffer.bind();
        mSurfelsLuminanceFramebuffer.viewport().apply();
        mSurfelsLuminanceFramebuffer.clear(GLFramebuffer::UnderlyingBuffer::Color | GLFramebuffer::UnderlyingBuffer::Depth);

        mSurfelLightingShader.bind();
        mSurfelLightingShader.setLight(directionalLight);
        mSurfelLightingShader.setMultibounceEnabled(mSettings.meshSettings.lightMultibounceEnabled);
        mSurfelLightingShader.ensureSamplerValidity([&]() {
            mSurfelLightingShader.setShadowCascades(mShadowCascades);
            mSurfelLightingShader.setExponentialShadowMap(*mDirectionalExponentialShadowMap);
            mSurfelLightingShader.setSurfelsGBuffer(*mSurfelData->surfelsGBuffer());
            mSurfelLightingShader.setGridProbesSHTextures(mGridProbesSHMaps);
            mSurfelLightingShader.setWorldBoundingBox(mScene->lightBakingVolume());
            mSurfelLightingShader.setProbePositions(*mDiffuseProbeData->probePositionsBufferTexture());
            mSurfelLightingShader.setSettings(mSettings);
        });

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }

    void DeferredSceneRenderer::averageSurfelClusterLuminances() {
        mSurfelClusterAveragingShader.bind();
        mSurfelClustersLuminanceFramebuffer.bind();
        mSurfelClustersLuminanceFramebuffer.viewport().apply();
        mSurfelClustersLuminanceFramebuffer.clear(GLFramebuffer::UnderlyingBuffer::Color | GLFramebuffer::UnderlyingBuffer::Depth);

        mSurfelClusterAveragingShader.ensureSamplerValidity([&]() {
            mSurfelClusterAveragingShader.setSurfelClustersGBuffer(*mSurfelData->surfelClustersGBuffer());
            mSurfelClusterAveragingShader.setSurfelsLuminaceMap(mSurfelsLuminanceMap);
        });

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }

    void DeferredSceneRenderer::updateGridProbes() {
        mGridProbesSHFramebuffer.bind();
        mGridProbesSHFramebuffer.viewport().apply();
        mGridProbesSHFramebuffer.clear(GLFramebuffer::UnderlyingBuffer::Color | GLFramebuffer::UnderlyingBuffer::Depth);

        mGridProbesUpdateShader.bind();
        mGridProbesUpdateShader.ensureSamplerValidity([&] {
            mGridProbesUpdateShader.setSurfelClustersLuminaceMap(mSurfelClustersLuminanceMap);
            mGridProbesUpdateShader.setProbeProjectionsMetadata(*mDiffuseProbeData->probeClusterProjectionsMetadataBufferTexture());
            mGridProbesUpdateShader.setProjectionClusterIndices(*mDiffuseProbeData->projectionClusterIndicesBufferTexture());
            mGridProbesUpdateShader.setProjectionClusterSphericalHarmonics(*mDiffuseProbeData->projectionClusterSHsBufferTexture());
            mGridProbesUpdateShader.setProbesGridResolution(mProbeGridResolution);
        });

        glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, (GLsizei)mProbeGridResolution.z);
    }

    void DeferredSceneRenderer::renderMeshes() {
        mCookTorranceShader.bind();

        mCookTorranceShader.setSettings(mSettings);
        mCookTorranceShader.setCamera(*(mScene->camera()));
        mCookTorranceShader.setLight(mScene->directionalLight());
        mCookTorranceShader.setWorldBoundingBox(mScene->lightBakingVolume());
        mCookTorranceShader.setProbePositions(*mDiffuseProbeData->probePositionsBufferTexture());
        mCookTorranceShader.setFrustumCascades(mShadowCascades);

        mCookTorranceShader.ensureSamplerValidity([&]() {
            mCookTorranceShader.setGBuffer(*mGBuffer);
            mCookTorranceShader.setExponentialShadowMap(*mDirectionalExponentialShadowMap);
            mCookTorranceShader.setGridProbesSHTextures(mGridProbesSHMaps);
        });

        mPostprocessTexturePool->redirectRenderingToTextures(mFrame, mThresholdFilteredOutputFrame);

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }

    void DeferredSceneRenderer::renderFinalImage(const GLFloatTexture2D& image) {
        bindDefaultFramebuffer();
        glDisable(GL_DEPTH_TEST);

        mFSQuadShader.bind();
        mFSQuadShader.setApplyToneMapping(false);
        mFSQuadShader.ensureSamplerValidity([&]() {
            mFSQuadShader.setTexture(image);
        });

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glEnable(GL_DEPTH_TEST);
    }

#pragma mark - Public interface

    void DeferredSceneRenderer::render() {
        mShadowCascades = mScene->directionalLight().cascadesForWorldBoundingBox(mScene->boundingBox());

        swapFrames();

        renderExponentialShadowMapsForDirectionalLight();
        relightSurfels();
        averageSurfelClusterLuminances();
        updateGridProbes();
        renderMeshes();

        auto ssrOutputTexture = mPostprocessTexturePool->claim();
        mSSREffect.applyReflections(*mScene->camera(), mPreviousFrame, mGBuffer, ssrOutputTexture, mPostprocessTexturePool);

//        auto bloomOutputTexture = mPostprocessTexturePool->claim();
//        mBloomEffect.bloom(mFrame, mThresholdFilteredOutputFrame, bloomOutputTexture, mPostprocessTexturePool, mSettings.bloomSettings);
//
//        auto toneMappingOutputTexture = mPostprocessTexturePool->claim();
//        mToneMappingEffect.toneMap(bloomOutputTexture, toneMappingOutputTexture, mPostprocessTexturePool);

        renderFinalImage(*ssrOutputTexture);

        mPostprocessTexturePool->putBack(ssrOutputTexture);
//        mPostprocessTexturePool->putBack(bloomOutputTexture);
//        mPostprocessTexturePool->putBack(toneMappingOutputTexture);
    }

}
