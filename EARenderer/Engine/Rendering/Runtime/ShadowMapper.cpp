//
//  ShadowMapper.cpp
//  EARenderer
//
//  Created by Pavel Muratov on 8/29/18.
//  Copyright © 2018 MPO. All rights reserved.
//

#include "ShadowMapper.hpp"

namespace EARenderer {

#pragma mark - Lifecycle

    ShadowMapper::ShadowMapper(const Scene *scene)
    :
    mScene(scene),
    mDepthRenderbuffer(Size2D(2048)),
    mDirectionalShadowMap(std::make_shared<GLFloatTexture2D<GLTexture::Float::RGBA32F>>(Size2D(2048))),
    mFramebuffer(std::make_shared<GLFramebuffer>(Size2D(2048))),
    mTexturePool(std::make_shared<PostprocessTexturePool<GLTexture::Float::RGBA32F>>(Size2D(2048))),
    mBlurEffect(mFramebuffer, mTexturePool)
    {
        if (scene->pointLights().size() > 0) {
            mOmnidirectionalShadowMaps = std::make_shared<GLFloatTextureCubemapArray<GLTexture::Float::R32F>>(Size2D(512), scene->pointLights().size());
        }

        mFramebuffer->attachRenderbuffer(mDepthRenderbuffer);

        size_t index = 0;
        for (ID pointLightID : scene->pointLights()) {
            mPointLightIDToArrayIndexMap[pointLightID] = index;
            index++;
        }
    }

#pragma mark - Setters

    void ShadowMapper::setRenderingSettings(const RenderingSettings& settings) {
        mSettings = settings;
    }

#pragma mark - Getters

    const FrustumCascades& ShadowMapper::cascades() const {
        return mShadowCascades;
    }

    size_t ShadowMapper::shadowMapIndexForPointLight(ID pointLightID) const {
        auto it = mPointLightIDToArrayIndexMap.find(pointLightID);
        if (it == mPointLightIDToArrayIndexMap.end()) {
            return -1;
        }
        return it->second;
    }

    std::shared_ptr<const GLFloatTexture2D<GLTexture::Float::RGBA32F>> ShadowMapper::directionalShadowMap() const {
        return mDirectionalShadowMap;
    }

    std::shared_ptr<const GLFloatTextureCubemapArray<GLTexture::Float::R32F>> ShadowMapper::omnidirectionalShadowMaps() const {
        return mOmnidirectionalShadowMaps;
    }

#pragma mark - Private Helpers

    void ShadowMapper::renderDirectionalShadowMaps() {
        auto renderTarget = mTexturePool->claim();
        mFramebuffer->redirectRenderingToTexturesMip(0, renderTarget);

        mDirectionalSMShader.bind();
        mDirectionalSMShader.setESMFactor(mSettings.meshSettings.ESMFactor);

        for (size_t cascade = 0; cascade < mShadowCascades.amount; cascade++) {
            // Ensure only one texture channel will be written to for each respective cascade
            glColorMask(cascade == 0, cascade == 1, cascade == 2, cascade == 3);

            mDirectionalSMShader.setLightMatrix(mShadowCascades.lightViewProjections[cascade]);

            for (ID meshInstanceID : mScene->meshInstances()) {
                auto& instance = mScene->meshInstances()[meshInstanceID];
                auto& subMeshes = ResourcePool::shared().meshes[instance.meshID()].subMeshes();

                mDirectionalSMShader.setModelMatrix(instance.transformation().modelMatrix());

                for (ID subMeshID : subMeshes) {
                    auto& subMesh = subMeshes[subMeshID];
                    subMesh.draw();
                }
            }

            // Prepare depth buffer for the next cascade rendering
            mFramebuffer->clear(GLFramebuffer::UnderlyingBuffer::Depth);
        }

        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

        mBlurEffect.blur(renderTarget, mDirectionalShadowMap, mSettings.meshSettings.shadowBlur);

        mTexturePool->putBack(renderTarget);
    }

    void ShadowMapper::renderOmnidirectionalShadowMaps() {

    }

#pragma mark - Rendering

    void ShadowMapper::render() {
        mShadowCascades = mScene->directionalLight().cascadesForBoundingBox(mScene->boundingBox(), 2);
        renderDirectionalShadowMaps();
        renderOmnidirectionalShadowMaps();
    }

}
