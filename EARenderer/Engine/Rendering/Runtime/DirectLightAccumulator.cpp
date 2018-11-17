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
        mCookTorranceShader.setLight(mScene->directionalLight());
        Drawable::TriangleStripQuad::Draw();
    }

    void DirectLightAccumulator::renderPointLights() {
        for (ID lightId : mScene->pointLights()) {

            const PointLight& light = mScene->pointLights()[lightId];
            size_t cubeArrayIndex = mShadowMapper->shadowMapIndexForPointLight(lightId);

            mCookTorranceShader.setLight(light);
            mCookTorranceShader.setShadowMapArrayIndex(cubeArrayIndex);

            Drawable::TriangleStripQuad::Draw();
        }
    }

#pragma mark - Getters / Setters

    void DirectLightAccumulator::setRenderingSettings(const RenderingSettings& settings) {
        mSettings = settings;
    }

#pragma mark - Public Interface

    void DirectLightAccumulator::render()
    {
        mCookTorranceShader.bind();
        mCookTorranceShader.setSettings(mSettings);
        mCookTorranceShader.setCamera(*(mScene->camera()));
        mCookTorranceShader.setFrustumCascades(mShadowMapper->cascades());
        mCookTorranceShader.ensureSamplerValidity([&]() {
            mCookTorranceShader.setGBuffer(*mGBuffer);
            mCookTorranceShader.setDirectionalShadowMapArray(*mShadowMapper->directionalShadowMapArray());
//            mCookTorranceShader.setExponentialShadowMaps(*mShadowMapper->omnidirectionalShadowMaps());
//            mCookTorranceShader.setExponentialShadowMap(*mShadowMapper->directionalShadowMap());
        });

        renderDirectionalLights();
        renderPointLights();
    }

}
