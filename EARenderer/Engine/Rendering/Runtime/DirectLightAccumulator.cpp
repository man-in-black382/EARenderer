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

    DirectLightAccumulator::DirectLightAccumulator(
            const Scene *scene,
            const SceneGBuffer *gBuffer,
            const ShadowMapper *shadowMapper,
            const GPUResourceController *gpuResourceController)
            : mScene(scene),
              mGBuffer(gBuffer),
              mShadowMapper(shadowMapper),
              mGPUResourceController(gpuResourceController) {
    }

#pragma mark -

    void DirectLightAccumulator::renderDirectionalLights() {
        mLightEvaluationShader.setLightType(LightType::Directional);

        if (!mScene->directionalLight().isEnabled()) {
            return;;
        }

        mLightEvaluationShader.setLight(mScene->directionalLight());
        mLightEvaluationShader.setFrustumCascades(mShadowMapper->cascades());
        mLightEvaluationShader.ensureSamplerValidity([&]() {
            mLightEvaluationShader.setDirectionalShadowMapArray(mShadowMapper->directionalShadowMapArray());
            mLightEvaluationShader.setPenumbra(mShadowMapper->directionalPenumbra());
            mLightEvaluationShader.setGBuffer(*mGBuffer);
        });
        Drawable::TriangleStripQuad::Draw();
    }

    void DirectLightAccumulator::renderPointLights() {
        mLightEvaluationShader.setLightType(LightType::Point);

        for (ID lightId : mScene->pointLights()) {
            const PointLight &light = mScene->pointLights()[lightId];

            if (!light.isEnabled()) {
                continue;
            }

            mLightEvaluationShader.setUniformBuffer(
                    ctcrc32("PointLightUBO"),
                    *mGPUResourceController->uniformBuffer(),
                    mGPUResourceController->pointLightUBODataLocation(lightId)
            );
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

        renderDirectionalLights();
        renderPointLights();
    }

}
