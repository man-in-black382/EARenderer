//
//  LightAccumulator.cpp
//  EARenderer
//
//  Created by Pavel Muratov on 9/3/18.
//  Copyright © 2018 MPO. All rights reserved.
//

#include "DirectLightAccumulator.hpp"

namespace EARenderer {

#pragma mark - Lifecycle

    DirectLightAccumulator::DirectLightAccumulator(const Scene *scene,
                                                   std::shared_ptr<const SceneGBuffer> gBuffer,
                                                   std::shared_ptr<const ShadowMapper> shadowMapper,
                                                   std::shared_ptr<GLFramebuffer> framebuffer)
    :
    mScene(scene),
    mGBuffer(gBuffer),
    mShadowMapper(shadowMapper),
    mFramebuffer(framebuffer),
    mLightBuffer(std::make_shared<GLFloatTexture2D<GLTexture::Float::RGBA16F>>(framebuffer->size()))
    { }

#pragma mark -

    void DirectLightAccumulator::renderDirectionalLights() {
        mCookTorranceShader.setLight(mScene->directionalLight());
        TriangleStripQuad::Draw();
    }

    void DirectLightAccumulator::renderPointLights() {
        for (ID lightId : mScene->pointLights()) {

            const PointLight& light = mScene->pointLights()[lightId];
            size_t cubeArrayIndex = mShadowMapper->shadowMapIndexForPointLight(lightId);

            mCookTorranceShader.setLight(light);
            mCookTorranceShader.setShadowMapArrayIndex(cubeArrayIndex);

            TriangleStripQuad::Draw();
        }
    }

#pragma mark - Getters / Setters

    void DirectLightAccumulator::setRenderingSettings(const RenderingSettings& settings) {
        mSettings = settings;
    }

    std::shared_ptr<GLFloatTexture2D<GLTexture::Float::RGBA16F>> DirectLightAccumulator::lightBuffer() {
        return mLightBuffer;
    }

#pragma mark - Public Interface

    void DirectLightAccumulator::render() {
        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_ONE);

        mFramebuffer->redirectRenderingToTextures(mLightBuffer);

        mCookTorranceShader.bind();
        mCookTorranceShader.setSettings(mSettings);
        mCookTorranceShader.setCamera(*(mScene->camera()));
        mCookTorranceShader.setFrustumCascades(mShadowMapper->cascades());
        mCookTorranceShader.ensureSamplerValidity([&]() {
            mCookTorranceShader.setGBuffer(*mGBuffer);
            mCookTorranceShader.setExponentialShadowMaps(*mShadowMapper->omnidirectionalShadowMaps());
            mCookTorranceShader.setExponentialShadowMap(*mShadowMapper->directionalShadowMap());
        });

        renderDirectionalLights();
        renderPointLights();

        glDisable(GL_BLEND);
    }

}
