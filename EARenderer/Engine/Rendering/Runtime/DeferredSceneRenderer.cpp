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
#include "Measurement.hpp"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

namespace EARenderer {

#pragma mark - Lifecycle

    DeferredSceneRenderer::DeferredSceneRenderer(
            const Scene *scene,
            const DefaultRenderComponentsProviding *provider,
            const RenderingSettings &settings,
            std::shared_ptr<const SurfelData> surfelData,
            std::shared_ptr<const DiffuseLightProbeData> diffuseProbeData,
            std::shared_ptr<const SceneGBuffer> GBuffer)
            :
            mScene(scene),
            mDefaultRenderComponentsProvider(provider),
            mSettings(settings),

            // Effects
            mFramebuffer(std::make_shared<GLFramebuffer>(settings.displayedFrameResolution)),
            mPostprocessTexturePool(std::make_shared<HalfPrecisionTexturePool>(settings.displayedFrameResolution)),
            mBloomEffect(mFramebuffer, mPostprocessTexturePool),
            mToneMappingEffect(mFramebuffer, mPostprocessTexturePool),
            mSSREffect(mFramebuffer, mPostprocessTexturePool),
            mSMAAEffect(mFramebuffer, mPostprocessTexturePool),

            // Helpers
            mSurfelData(surfelData),
            mProbeData(diffuseProbeData),
            mShadowMapper(std::make_shared<ShadowMapper>(scene, GBuffer, settings.meshSettings.shadowCascadesCount)),
            mDirectLightAccumulator(std::make_shared<DirectLightAccumulator>(scene, GBuffer, mShadowMapper)),
            mIndirectLightAccumulator(std::make_shared<IndirectLightAccumulator>(scene, GBuffer, surfelData, diffuseProbeData, mShadowMapper)),
            mGBuffer(GBuffer) {

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

    void DeferredSceneRenderer::setRenderingSettings(const RenderingSettings &settings) {
        mSettings = settings;
        mShadowMapper->setRenderingSettings(settings);
        mDirectLightAccumulator->setRenderingSettings(settings);
        mIndirectLightAccumulator->setRenderingSettings(settings);
    }

#pragma mark - Getters

    std::shared_ptr<const std::array<GLLDRTexture3D, 4>> DeferredSceneRenderer::gridProbesSphericalHarmonics() const {
        return mIndirectLightAccumulator->gridProbesSphericalHarmonics();
    }

    std::shared_ptr<const GLFloatTexture2D<GLTexture::Float::R16F>> DeferredSceneRenderer::surfelsLuminanceMap() const {
        return mIndirectLightAccumulator->surfelsLuminanceMap();
    }

    std::shared_ptr<const GLFloatTexture2D<GLTexture::Float::R16F>> DeferredSceneRenderer::surfelClustersLuminanceMap() const {
        return mIndirectLightAccumulator->surfelClustersLuminanceMap();
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
            auto &instance = mScene->meshInstances()[instanceID];
            auto &subMeshes = ResourcePool::shared().mesh(instance.meshID()).subMeshes();

            mDepthPrepassShader.setModelMatrix(instance.transformation().modelMatrix());

            for (ID subMeshID : subMeshes) {
                auto &subMesh = subMeshes[subMeshID];
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
            mSkyboxShader.setExposure(mScene->skybox()->exposure());
        });

        mScene->skybox()->draw();
    }

    void DeferredSceneRenderer::renderFinalImage(std::shared_ptr<HalfPrecisionTexturePool::PostprocessTexture> image) {
        bindDefaultFramebuffer();
        glDisable(GL_DEPTH_TEST);

        mFSQuadShader.bind();
        mFSQuadShader.setApplyToneMapping(false);
        mFSQuadShader.ensureSamplerValidity([&]() {
            mFSQuadShader.setTexture(*image);
        });

        Drawable::TriangleStripQuad::Draw();
        glEnable(GL_DEPTH_TEST);
    }

#pragma mark - Public interface

    void DeferredSceneRenderer::render(const DebugOpportunity &debugClosure) {

        glFinish();
        Measurement::ExecutionTime("Shadow and penumbra", [&] {
            mShadowMapper->render();
            glFinish();
        });

        Measurement::ExecutionTime("Probe update", [&] {
            mIndirectLightAccumulator->updateProbes();
            glFinish();
        });

        // We're using depth buffer rendered during GBufferCookTorrance construction.
        // Depth writes are disabled for the purpose of combining skybox
        // and debugging entities with full screen deferred rendering:
        // mMeshes are rendered as a full screen quad, then skybox is rendered
        // where it should, using depth buffer filled by geometry.
        // Then, full screen quad rendering (postprocessing) can be applied without
        // polluting the depth buffer, which leaves us an ability to render 3D debug entities,
        // like light probe spheres, surfels etc.
        glDepthMask(GL_FALSE);

        auto lightAccumulationTarget = mPostprocessTexturePool->claim();
        mFramebuffer->redirectRenderingToTextures(GLFramebuffer::UnderlyingBuffer::Color, lightAccumulationTarget);

        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_ONE);
        glDisable(GL_DEPTH_TEST);

        Measurement::ExecutionTime("Direct light accumulation", [&] {
            mDirectLightAccumulator->render();
            glFinish();
        });

        Measurement::ExecutionTime("Indirect light accumulation", [&] {
            mIndirectLightAccumulator->render();
            glFinish();
        });

        glDisable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);

        renderSkybox();

        auto ssrBaseOutputTexture = mPostprocessTexturePool->claim(); // Frame with reflections applied
        auto ssrBrightOutputTexture = mPostprocessTexturePool->claim(); // Frame filtered by luminosity threshold and suitable for bloom effect

        Measurement::ExecutionTime("SSR", [&] {
            mSSREffect.applyReflections(*mScene->camera(), mGBuffer, lightAccumulationTarget, ssrBaseOutputTexture, ssrBrightOutputTexture);
            glFinish();
        });

        auto bloomOutputTexture = lightAccumulationTarget;

        Measurement::ExecutionTime("Bloom", [&] {
            mBloomEffect.bloom(ssrBaseOutputTexture, ssrBrightOutputTexture, bloomOutputTexture, mSettings.bloomSettings);
            glFinish();
        });

        glDepthMask(GL_TRUE);

        debugClosure();

        auto toneMappingOutputTexture = ssrBaseOutputTexture;

        Measurement::ExecutionTime("Tone mapping", [&] {
            mToneMappingEffect.toneMap(bloomOutputTexture, toneMappingOutputTexture);
            glFinish();
        });
//
//        auto smaaOutputTexture = ssrBrightOutputTexture;
//
//        Measurement::ExecutionTime("Antialiasing", [&] {
//            mSMAAEffect.antialise(toneMappingOutputTexture, smaaOutputTexture);
//            glFinish();
//        });

//        // DEBUG
//        glDisable(GL_DEPTH_TEST);
//
//        mFSQuadShader.bind();
//        mFSQuadShader.setApplyToneMapping(false);
//        mFSQuadShader.ensureSamplerValidity([&]() {
//            mFSQuadShader.setTexture(mShadowMapper->penumbraForPointLight(*mScene->pointLights().begin()));
//        });

//        Drawable::TriangleStripQuad::Draw();
//        glEnable(GL_DEPTH_TEST);
//        // DEBUG

//        renderFinalImage(smaaOutputTexture);
        renderFinalImage(toneMappingOutputTexture);

        mPostprocessTexturePool->putBack(lightAccumulationTarget);
        mPostprocessTexturePool->putBack(ssrBaseOutputTexture);
        mPostprocessTexturePool->putBack(ssrBrightOutputTexture);
    }

}
