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
                                                   std::shared_ptr<const ShadowMapper> shadowMapper)
    :
    mScene(scene),
    mGBuffer(gBuffer),
    mShadowMapper(shadowMapper)
    { }

#pragma mark -

    void DirectLightAccumulator::renderDirectionalLights() {
        if (!mScene->directionalLight().isEnabled()) {
            return;;
        }

        mCookTorranceShader.setLight(mScene->directionalLight());
        mCookTorranceShader.ensureSamplerValidity([&]() {
            mCookTorranceShader.setDirectionalShadowMapArray(mShadowMapper->directionalShadowMapArray());
            mCookTorranceShader.setPenumbra(mShadowMapper->directionalPenumbra());
        });
        Drawable::TriangleStripQuad::Draw();
    }

    void DirectLightAccumulator::renderPointLights() {
        for (ID lightId : mScene->pointLights()) {
            const PointLight& light = mScene->pointLights()[lightId];

            if (!light.isEnabled()) {
                continue;
            }

            mCookTorranceShader.setLight(light);
            mCookTorranceShader.ensureSamplerValidity([&]() {
                mCookTorranceShader.setOmnidirectionalShadowCubemap(mShadowMapper->shadowMapForPointLight(lightId));
                mCookTorranceShader.setPenumbra(mShadowMapper->penumbraForPointLight(lightId));
            });
            Drawable::TriangleStripQuad::Draw();
        }
    }

#pragma mark - Getters / Setters

    void DirectLightAccumulator::setRenderingSettings(const RenderingSettings& settings) {
        mSettings = settings;
    }

#pragma mark - Public Interface

    void DirectLightAccumulator::render() {
        mCookTorranceShader.bind();
        mCookTorranceShader.setSettings(mSettings);
        mCookTorranceShader.setCamera(*(mScene->camera()));
        mCookTorranceShader.setFrustumCascades(mShadowMapper->cascades());
        mCookTorranceShader.ensureSamplerValidity([&]() {
            mCookTorranceShader.setGBuffer(*mGBuffer);
        });

        renderDirectionalLights();
        renderPointLights();
    }

}
