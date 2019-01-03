//
//  LightAccumulator.cpp
//  EARenderer
//
//  Created by Pavel Muratov on 9/3/18.
//  Copyright © 2018 MPO. All rights reserved.
//

#include "DirectLightAccumulator.hpp"
#include "Drawable.hpp"

namespace EARenderer {

#pragma mark - Lifecycle

    DirectLightAccumulator::DirectLightAccumulator(const Scene *scene, std::shared_ptr<const SceneGBuffer> gBuffer, std::shared_ptr<const ShadowMapper> shadowMapper)
            : mScene(scene), mGBuffer(gBuffer), mShadowMapper(shadowMapper) {}

#pragma mark -

    void DirectLightAccumulator::renderDirectionalLights() {
        if (!mScene->directionalLight().isEnabled()) {
            return;;
        }

        mLightEvaluationShader.setLight(mScene->directionalLight());
        mLightEvaluationShader.ensureSamplerValidity([&]() {
            mLightEvaluationShader.setDirectionalShadowMapArray(mShadowMapper->directionalShadowMapArray());
            mLightEvaluationShader.setPenumbra(mShadowMapper->directionalPenumbra());
            mLightEvaluationShader.setGBuffer(*mGBuffer);
        });
        Drawable::TriangleStripQuad::Draw();
    }

    void DirectLightAccumulator::renderPointLights() {
        for (ID lightId : mScene->pointLights()) {
            const PointLight &light = mScene->pointLights()[lightId];

            if (!light.isEnabled()) {
                continue;
            }

            mLightEvaluationShader.setLight(light);
            mLightEvaluationShader.ensureSamplerValidity([&]() {
                mLightEvaluationShader.setOmnidirectionalShadowCubemap(mShadowMapper->shadowMapForPointLight(lightId));
                mLightEvaluationShader.setPenumbra(mShadowMapper->penumbraForPointLight(lightId));
                mLightEvaluationShader.setGBuffer(*mGBuffer);
            });
            Drawable::TriangleStripQuad::Draw();
        }
    }

#pragma mark - Getters / Setters

    void DirectLightAccumulator::setRenderingSettings(const RenderingSettings &settings) {
        mSettings = settings;
    }

#pragma mark - Public Interface

    void DirectLightAccumulator::render() {
        mLightEvaluationShader.bind();
        mLightEvaluationShader.setSettings(mSettings);
        mLightEvaluationShader.setCamera(*(mScene->camera()));
        mLightEvaluationShader.setFrustumCascades(mShadowMapper->cascades());

        renderDirectionalLights();
        renderPointLights();
    }

}
