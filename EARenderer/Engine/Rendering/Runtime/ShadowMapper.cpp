//
//  ShadowMapper.cpp
//  EARenderer
//
//  Created by Pavel Muratov on 8/29/18.
//  Copyright © 2018 MPO. All rights reserved.
//

#include "ShadowMapper.hpp"
#include "Drawable.hpp"
#include "SharedResourceStorage.hpp"
#include "LogUtils.hpp"

namespace EARenderer {

#pragma mark - Lifecycle

    ShadowMapper::ShadowMapper(
            const Scene *scene,
            const SharedResourceStorage *resourceStorage,
            const GPUResourceController *gpuResourceController,
            const SceneGBuffer *gBuffer,
            uint8_t cascadeCount)
            :
            mScene(scene),
            mGBuffer(gBuffer),
            mGPUResourceController(gpuResourceController),
            mResourceStorage(resourceStorage),
            mCascadeCount(cascadeCount),
            mShadowFramebuffer(mSettings.directionalShadowMapResolution),
            mOmnidirectionalShadowFramebuffer(mSettings.omnidirectionalShadowMapResolution),
            mPenumbraFramebuffer(mSettings.penumbraResolution),
            mTexturePool(mSettings.penumbraResolution),
            mDirectionalPenumbra(mSettings.penumbraResolution),
            mDirectionalShadowMapArray(mSettings.directionalShadowMapResolution, std::min(cascadeCount, MaximumCascadeCount), Sampling::ComparisonMode::ReferenceToTexture),
            mBlurEffect(&mPenumbraFramebuffer, &mTexturePool),
            mBilinearSampler(Sampling::Filter::Bilinear, Sampling::WrapMode::ClampToEdge, Sampling::ComparisonMode::None) {

        for (ID pointLightID : scene->pointLights()) {
            mOmnidirectionalPenumbras.emplace(
                    std::piecewise_construct,
                    std::forward_as_tuple(pointLightID),
                    std::forward_as_tuple(mSettings.penumbraResolution)
            );
            mOmnidirectionalShadowMaps.emplace(
                    std::piecewise_construct,
                    std::forward_as_tuple(pointLightID),
                    std::forward_as_tuple(mSettings.omnidirectionalShadowMapResolution, Sampling::ComparisonMode::ReferenceToTexture)
            );
        }
    }

#pragma mark - Setters

    void ShadowMapper::setRenderingSettings(const RenderingSettings &settings) {
        mSettings = settings;
    }

#pragma mark - Getters

    const FrustumCascades &ShadowMapper::cascades() const {
        return mShadowCascades;
    }

    const GLDepthTextureCubemap &ShadowMapper::shadowMapForPointLight(ID pointLightID) const {
        auto it = mOmnidirectionalShadowMaps.find(pointLightID);
        if (it == mOmnidirectionalShadowMaps.end()) {
            throw std::invalid_argument("Shadow map not found");
        }
        return it->second;
    }

    const GLFloatTexture2D<GLTexture::Float::R16F> &ShadowMapper::penumbraForPointLight(ID pointLightID) const {
        auto it = mOmnidirectionalPenumbras.find(pointLightID);
        if (it == mOmnidirectionalPenumbras.end()) {
            throw std::invalid_argument("Penumbra not found");
        }
        return it->second;
    }

    const GLDepthTexture2DArray &ShadowMapper::directionalShadowMapArray() const {
        return mDirectionalShadowMapArray;
    }

    const GLFloatTexture2D<GLTexture::Float::R16F> &ShadowMapper::directionalPenumbra() const {
        return mDirectionalPenumbra;
    }

#pragma mark - Private Helpers

    void ShadowMapper::renderDirectionalShadowMaps() {
        if (!mScene->sun().isEnabled()) {
            return;
        }

        mShadowMapShader.bind();
        mShadowMapShader.setViewProjectionMatrices(mShadowCascades.lightViewProjections);

        mShadowFramebuffer.bind();
        mShadowFramebuffer.attachDepthTexture(mDirectionalShadowMapArray);
        GLViewport(mSettings.directionalShadowMapResolution).apply();
        mShadowFramebuffer.clear(GLFramebuffer::UnderlyingBuffer::Depth);

        for (ID meshInstanceID : mScene->meshInstances()) {
            const auto &instance = mScene->meshInstances()[meshInstanceID];
            const auto &subMeshes = mResourceStorage->mesh(instance.meshID()).subMeshes();

            mShadowMapShader.setModelMatrix(instance.transformation().modelMatrix());

            for (ID subMeshID : subMeshes) {
                const auto &subMesh = subMeshes[subMeshID];
                const auto &location = mGPUResourceController->subMeshVBODataLocation(instance.meshID(), subMeshID);
                Drawable::TriangleMesh::DrawInstanced(mShadowCascades.amount, location);
            }
        }
    }

    void ShadowMapper::renderOmnidirectionalShadowMaps() {
        mShadowMapShader.bind();

        mShadowFramebuffer.bind();
        GLViewport(mSettings.omnidirectionalShadowMapResolution).apply();

        for (ID pointLightID : mScene->pointLights()) {
            // Setup 6 view-projection matrices to capture geometry from 6 perspectives
            const PointLight &light = mScene->pointLights()[pointLightID];

            if (!light.isEnabled()) {
                continue;
            }

            mShadowFramebuffer.attachDepthTexture(shadowMapForPointLight(pointLightID));
            mShadowFramebuffer.clear(GLFramebuffer::UnderlyingBuffer::Depth);

            auto matrices = light.viewProjectionMatrices();
            mShadowMapShader.setViewProjectionMatrices({matrices.begin(), matrices.end()});

            for (ID meshInstanceID : mScene->meshInstances()) {
                auto &instance = mScene->meshInstances()[meshInstanceID];
                auto &subMeshes = mResourceStorage->mesh(instance.meshID()).subMeshes();

                mShadowMapShader.setModelMatrix(instance.transformation().modelMatrix());

                for (ID subMeshID : subMeshes) {
                    const auto &subMesh = subMeshes[subMeshID];
                    const auto &location = mGPUResourceController->subMeshVBODataLocation(instance.meshID(), subMeshID);
                    Drawable::TriangleMesh::DrawInstanced(6, location); // 6 for 6 cubemap faces
                }
            }
        }
    }

    void ShadowMapper::renderDirectionalPenumbra() {
        if (!mScene->sun().isEnabled()) {
            return;
        }

        mPenumbraFramebuffer.redirectRenderingToTextures(GLFramebuffer::UnderlyingBuffer::None, &mDirectionalPenumbra);

        mDirectionalPenumbraGenerationShader.bind();
        mDirectionalPenumbraGenerationShader.setCamera(*mScene->camera());
        mDirectionalPenumbraGenerationShader.setLight(mScene->sun());
        mDirectionalPenumbraGenerationShader.ensureSamplerValidity([&] {
            mDirectionalPenumbraGenerationShader.setGBuffer(*mGBuffer);
            mDirectionalPenumbraGenerationShader.setFrustumCascades(mShadowCascades);
            mDirectionalPenumbraGenerationShader.setDirectionalShadowMapArray(mDirectionalShadowMapArray, mBilinearSampler);
        });

        Drawable::TriangleStripQuad::Draw();
    }

    void ShadowMapper::renderOmnidirectionalPenumbras() {
        mOmnidirectionalPenumbraGenerationShader.bind();
        mOmnidirectionalPenumbraGenerationShader.setCamera(*mScene->camera());

        for (ID lightID : mScene->pointLights()) {
            const PointLight &light = mScene->pointLights()[lightID];

            if (!light.isEnabled()) {
                continue;
            }

            auto &penumbra = penumbraForPointLight(lightID);
            auto &shadowMap = shadowMapForPointLight(lightID);

            mPenumbraFramebuffer.redirectRenderingToTextures(GLFramebuffer::UnderlyingBuffer::None, &penumbra);

            mOmnidirectionalPenumbraGenerationShader.setUniformBuffer(
                    ctcrc32("PointLightUBO"),
                    *mGPUResourceController->uniformBuffer(),
                    mGPUResourceController->pointLightUBODataLocation(lightID)
            );

            mOmnidirectionalPenumbraGenerationShader.ensureSamplerValidity([&] {
                mOmnidirectionalPenumbraGenerationShader.setGBuffer(*mGBuffer);
                mOmnidirectionalPenumbraGenerationShader.setShadowMap(shadowMap, mBilinearSampler);
            });

            Drawable::TriangleStripQuad::Draw();
        }

//        for (auto& pair : mOmnidirectionalPenumbras) {
//            auto &penumbra = it.second;
//            mBlurEffect.blur(penumbra, <#EARenderer::PostprocessTexturePool::PostprocessTexture & outputImage#>, { 3, 0.84, 0, 0 })
//        }
    }

#pragma mark - Rendering

    void ShadowMapper::render() {
        mGPUResourceController->meshVAO()->bind();
        mShadowCascades = mScene->sun().cascadesForBoundingBox(mScene->boundingBox(), mCascadeCount);
//        mShadowCascades = mScene->sun().cascadesForCamera(*mScene->camera(), 1);

        renderOmnidirectionalShadowMaps();
        renderDirectionalShadowMaps();
        renderOmnidirectionalPenumbras();
//        renderDirectionalPenumbra();
    }

}
