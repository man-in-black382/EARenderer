//
//  ShadowMapper.cpp
//  EARenderer
//
//  Created by Pavel Muratov on 8/29/18.
//  Copyright © 2018 MPO. All rights reserved.
//

#include "ShadowMapper.hpp"
#include "ResourcePool.hpp"

namespace EARenderer {

#pragma mark - Lifecycle

    ShadowMapper::ShadowMapper(const Scene *scene, uint8_t cascadeCount)
    :
    mScene(scene),
    mCascadeCount(cascadeCount),
    mFramebuffer(std::make_shared<GLFramebuffer>(Size2D(3072))),
    mDirectionalShadowMapArray(std::make_shared<GLDepthTexture2DArray>(mFramebuffer->size(),
                                                                       std::min(cascadeCount, MaximumCascadeCount),
                                                                       GLTexture::ComparisonMode::ReferenceToTexture))
    {
//        if (scene->pointLights().size() > 0) {
//            mOmnidirectionalShadowMaps = std::make_shared<GLFloatTextureCubemapArray<GLTexture::Float::R32F>>(Size2D(512), scene->pointLights().size());
//        }
        
        mFramebuffer->attachDepthTexture(*mDirectionalShadowMapArray);
        
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
    
    std::shared_ptr<const GLDepthTexture2DArray> ShadowMapper::directionalShadowMapArray() const {
        return mDirectionalShadowMapArray;
    }

#pragma mark - Private Helpers

    void ShadowMapper::renderDirectionalShadowMaps() {
        mDirectionalShadowMapShader.bind();
        mDirectionalShadowMapShader.setCascades(mShadowCascades);
        
        mFramebuffer->bind();
        mFramebuffer->viewport().apply();
        mFramebuffer->clear(GLFramebuffer::UnderlyingBuffer::Depth);
        
        for (ID meshInstanceID : mScene->meshInstances()) {
            auto& instance = mScene->meshInstances()[meshInstanceID];
            auto& subMeshes = ResourcePool::shared().meshes[instance.meshID()].subMeshes();
            
            mDirectionalShadowMapShader.setModelMatrix(instance.transformation().modelMatrix());
            
            for (ID subMeshID : subMeshes) {
                auto& subMesh = subMeshes[subMeshID];
                subMesh.drawInstanced(mShadowCascades.amount);
            }
        }
    }
    
    void ShadowMapper::renderOmnidirectionalShadowMaps() {
        
    }

#pragma mark - Rendering

    void ShadowMapper::render() {
        ResourcePool::shared().VAO().bind();
        mShadowCascades = mScene->directionalLight().cascadesForBoundingBox(mScene->boundingBox(), mCascadeCount);
        renderDirectionalShadowMaps();
    }

}
