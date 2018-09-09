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

    // Effects
    mFramebuffer(std::make_shared<GLFramebuffer>(settings.resolution)),
    mPostprocessTexturePool(std::make_shared<HalfPrecisionTexturePool>(settings.resolution)),
    mBloomEffect(mFramebuffer, mPostprocessTexturePool),
    mToneMappingEffect(mFramebuffer, mPostprocessTexturePool),
    mSSREffect(mFramebuffer, mPostprocessTexturePool),

    // Helpers
    mProbeData(diffuseProbeData),
    mShadowMapper(std::make_shared<ShadowMapper>(scene)),
    mDirectLightAccumulator(std::make_shared<DirectLightAccumulator>(scene, GBuffer, mShadowMapper, mFramebuffer)),
    mIndirectLightAccumulator(std::make_shared<IndirectLightAccumulator>(scene, surfelData, diffuseProbeData, mShadowMapper)),
    mGBuffer(GBuffer),
    mProbeGridResolution(scene->preferredProbeGridResolution()),

    // Output frame
    mFrame(mPostprocessTexturePool->claim()),
    mThresholdFilteredOutputFrame(mPostprocessTexturePool->claim())
    {
        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
        glDisable(GL_BLEND);
        glClearColor(0.0, 0.0, 0.0, 1.0);
        glClearDepth(1.0);
        glDepthFunc(GL_LEQUAL);
        
        mFramebuffer->attachDepthTexture(*mGBuffer->depthBuffer);
    }

#pragma mark - Setters

    void DeferredSceneRenderer::setRenderingSettings(const RenderingSettings& settings) {
        mSettings = settings;
        mShadowMapper->setRenderingSettings(settings);
        mDirectLightAccumulator->setRenderingSettings(settings);
        mIndirectLightAccumulator->setRenderingSettings(settings);
    }

#pragma mark - Rendering
#pragma mark - Runtime

    void DeferredSceneRenderer::bindDefaultFramebuffer() {
        if (mDefaultRenderComponentsProvider) {
            mDefaultRenderComponentsProvider->bindSystemFramebuffer();
            mDefaultRenderComponentsProvider->defaultViewport().apply();
        }
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
    
    void DeferredSceneRenderer::renderSkybox() {
        mSkyboxShader.bind();
        mSkyboxShader.ensureSamplerValidity([this]() {
            mSkyboxShader.setViewMatrix(mScene->camera()->viewMatrix());
            mSkyboxShader.setProjectionMatrix(mScene->camera()->projectionMatrix());
            mSkyboxShader.setEquirectangularMap(*mScene->skybox()->equirectangularMap());
        });

        mScene->skybox()->draw();
    }

    void DeferredSceneRenderer::composeLightBuffers(std::shared_ptr<HalfPrecisionTexturePool::PostprocessTexture> reflections) {
        mLightComposingShader.bind();
        mLightComposingShader.ensureSamplerValidity([&]() {
            mLightComposingShader.setCamera(*mScene->camera());
            mLightComposingShader.setGBuffer(*mGBuffer);
            mLightComposingShader.setLightBuffer(*mDirectLightAccumulator->lightBuffer());
            mLightComposingShader.setProbePositions(*mProbeData->probePositionsBufferTexture());
            mLightComposingShader.setWorldBoundingBox(mScene->lightBakingVolume());
            mLightComposingShader.setGridProbesSHTextures(*mIndirectLightAccumulator->gridProbesSphericalHarmonics());
            mLightComposingShader.setReflections(*reflections);
        });

        TriangleStripQuad::Draw();
    }

    void DeferredSceneRenderer::renderFinalImage(std::shared_ptr<HalfPrecisionTexturePool::PostprocessTexture> image) {
        bindDefaultFramebuffer();
        glDisable(GL_DEPTH_TEST);

        mFSQuadShader.bind();
        mFSQuadShader.setApplyToneMapping(false);
        mFSQuadShader.ensureSamplerValidity([&]() {
            mFSQuadShader.setTexture(*image);
        });

        TriangleStripQuad::Draw();
        glEnable(GL_DEPTH_TEST);
    }

#pragma mark - Public interface

    void DeferredSceneRenderer::render() {

        mShadowMapper->render();
        mIndirectLightAccumulator->render();

        // We're using depth buffer rendered during gbuffer construction.
        // Depth writes are disabled for the purpose of combining skybox
        // and debugging entities with full screen deferred rendering:
        // meshes are rendered as a full screen quad, then skybox is rendered
        // where it should, using depth buffer filled by geometry.
        // Then, full screen quad rendering (postprocessing) can be applied without
        // polluting the depth buffer, which leaves us an ability to render 3D debug stuff,
        // like light probe spheres, surfels etc.
        glDepthMask(GL_FALSE);

        mDirectLightAccumulator->render();
        renderSkybox();

        auto reflectionsOutputTexture = mPostprocessTexturePool->claim();
        mSSREffect.applyReflections(*mScene->camera(), mGBuffer, mDirectLightAccumulator->lightBuffer(), reflectionsOutputTexture);

        mFramebuffer->redirectRenderingToTexturesMip(0, mFrame, mThresholdFilteredOutputFrame);
        composeLightBuffers(reflectionsOutputTexture);

        auto bloomOutputTexture = mPostprocessTexturePool->claim();
        mBloomEffect.bloom(mFrame, mThresholdFilteredOutputFrame, bloomOutputTexture, mSettings.bloomSettings);

        auto toneMappingOutputTexture = mPostprocessTexturePool->claim();
        mToneMappingEffect.toneMap(bloomOutputTexture, toneMappingOutputTexture);

        renderFinalImage(toneMappingOutputTexture);

        mPostprocessTexturePool->putBack(reflectionsOutputTexture);
        mPostprocessTexturePool->putBack(bloomOutputTexture);
        mPostprocessTexturePool->putBack(toneMappingOutputTexture);

        glDepthMask(GL_TRUE);
    }

}
