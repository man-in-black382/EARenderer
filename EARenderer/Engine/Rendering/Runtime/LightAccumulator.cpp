//
//  LightAccumulator.cpp
//  EARenderer
//
//  Created by Pavel Muratov on 9/3/18.
//  Copyright © 2018 MPO. All rights reserved.
//

#include "LightAccumulator.hpp"

namespace EARenderer {

#pragma mark - Lifecycle

    LightAccumulator::LightAccumulator(Scene *scene,
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

    void LightAccumulator::renderDirectionalLights() {
        mCookTorranceShader.setLight(mScene->directionalLight());
        TriangleStripQuad::Draw();
    }

    void LightAccumulator::renderPointLights() {
        for (ID lightId : mScene->pointLights()) {

            PointLight& light = mScene->pointLights()[lightId];
            size_t cubeArrayIndex = mShadowMapper->shadowMapIndexForPointLight(lightId);

            mCookTorranceShader.setLight(light);
            mCookTorranceShader.setShadowMapArrayIndex(cubeArrayIndex);

            TriangleStripQuad::Draw();
        }
    }

    void LightAccumulator::setSettings(const RenderingSettings& settings) {
        mSettings = settings;
    }

    void LightAccumulator::accumulateDirectLighting() {
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
