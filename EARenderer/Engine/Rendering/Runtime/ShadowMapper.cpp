//
//  ShadowMapper.cpp
//  EARenderer
//
//  Created by Pavel Muratov on 8/29/18.
//  Copyright © 2018 MPO. All rights reserved.
//

#include "ShadowMapper.hpp"
#include "ResourcePool.hpp"
#include "Log.hpp"

namespace EARenderer {
    
#pragma mark - Lifecycle
    
    ShadowMapper::ShadowMapper(const Scene *scene, std::shared_ptr<const SceneGBuffer> gBuffer, uint8_t cascadeCount)
    :
    mScene(scene),
    mGBuffer(gBuffer),
    mCascadeCount(cascadeCount),
    mShadowFramebuffer(mSettings.directionalShadowMapResolution),
    mOmnidirectionalShadowFramebuffer(mSettings.omnidirectionalShadowMapResolution),
    mPenumbraFramebuffer(mSettings.penumbraResolution),
    mDirectionalPenumbra(mSettings.penumbraResolution),
    mDirectionalShadowMapArray(mSettings.directionalShadowMapResolution, std::min(cascadeCount, MaximumCascadeCount), Sampling::ComparisonMode::ReferenceToTexture),
    mBilinearSampler(Sampling::Filter::Bilinear, Sampling::WrapMode::ClampToEdge, Sampling::ComparisonMode::None)
    {
        mShadowFramebuffer.attachDepthTexture(mDirectionalShadowMapArray);

        for (ID pointLightID : scene->pointLights()) {
            mOmnidirectionalPenumbras.emplace(pointLightID, mSettings.penumbraResolution);
            mOmnidirectionalShadowMaps.emplace(
                    std::piecewise_construct,
                    std::forward_as_tuple(pointLightID),
                    std::forward_as_tuple(mSettings.omnidirectionalShadowMapResolution, Sampling::ComparisonMode::ReferenceToTexture)
            );
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
    
    const GLDepthTextureCubemap& ShadowMapper::shadowMapForPointLight(ID pointLightID) const {
        auto it = mOmnidirectionalShadowMaps.find(pointLightID);
        if (it == mOmnidirectionalShadowMaps.end()) {
            throw std::invalid_argument("Shadow map not found");
        }
        return it->second;
    }
    
    const GLNormalizedTexture2D<GLTexture::Normalized::R>& ShadowMapper::penumbraForPointLight(ID pointLightID) const {
        auto it = mOmnidirectionalPenumbras.find(pointLightID);
        if (it == mOmnidirectionalPenumbras.end()) {
            throw std::invalid_argument("Penumbra not found");
        }
        return it->second;
    }
    
    const GLDepthTexture2DArray& ShadowMapper::directionalShadowMapArray() const {
        return mDirectionalShadowMapArray;
    }
    
    const GLNormalizedTexture2D<GLTexture::Normalized::R>& ShadowMapper::directionalPenumbra() const {
        return mDirectionalPenumbra;
    }
    
#pragma mark - Private Helpers
    
    void ShadowMapper::renderDirectionalPenumbra() {
        if (!mScene->directionalLight().isEnabled()) {
            return;
        }

        mPenumbraFramebuffer.redirectRenderingToTextures(GLFramebuffer::UnderlyingBuffer::None, &mDirectionalPenumbra);
        
        mDirectionalPenumbraGenerationShader.bind();
        mDirectionalPenumbraGenerationShader.setCamera(*mScene->camera());
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
            const PointLight& light = mScene->pointLights()[lightID];

            if (!light.isEnabled()) {
                continue;
            }

            auto& penumbra = penumbraForPointLight(lightID);
            auto& shadowMap = shadowMapForPointLight(lightID);

            mPenumbraFramebuffer.redirectRenderingToTextures(GLFramebuffer::UnderlyingBuffer::None, &penumbra);

            mOmnidirectionalPenumbraGenerationShader.setLight(light);
            mOmnidirectionalPenumbraGenerationShader.ensureSamplerValidity([&] {
                mOmnidirectionalPenumbraGenerationShader.setGBuffer(*mGBuffer);
                mOmnidirectionalPenumbraGenerationShader.setShadowMap(shadowMap, mBilinearSampler);
            });

            Drawable::TriangleStripQuad::Draw();
        }
    }

    void ShadowMapper::renderDirectionalShadowMaps() {
        if (!mScene->directionalLight().isEnabled()) {
            return;
        }

        mShadowMapShader.bind();
        mShadowMapShader.setViewProjectionMatrices(mShadowCascades.lightViewProjections);
        
        mShadowFramebuffer.bind();
        mShadowFramebuffer.attachDepthTexture(mDirectionalShadowMapArray);
        GLViewport(mSettings.directionalShadowMapResolution).apply();
        mShadowFramebuffer.clear(GLFramebuffer::UnderlyingBuffer::Depth);
        
        for (ID meshInstanceID : mScene->meshInstances()) {
            auto& instance = mScene->meshInstances()[meshInstanceID];
            auto& subMeshes = ResourcePool::shared().meshes[instance.meshID()].subMeshes();
            
            mShadowMapShader.setModelMatrix(instance.transformation().modelMatrix());
            
            for (ID subMeshID : subMeshes) {
                auto& subMesh = subMeshes[subMeshID];
                subMesh.drawInstanced(mShadowCascades.amount);
            }
        }
    }
    
    void ShadowMapper::renderOmnidirectionalShadowMaps() {
        mShadowMapShader.bind();
                
        mShadowFramebuffer.bind();
        GLViewport(mSettings.omnidirectionalShadowMapResolution).apply();
        
        for (ID pointLightID : mScene->pointLights()) {
            // Setup 6 view-projection matrices to capture geometry from 6 perspectives
            const PointLight& light = mScene->pointLights()[pointLightID];

            if (!light.isEnabled()) {
                continue;
            }

            mShadowFramebuffer.attachDepthTexture(shadowMapForPointLight(pointLightID));
            mShadowFramebuffer.clear(GLFramebuffer::UnderlyingBuffer::Depth);

            auto matrices = light.viewProjectionMatrices();
            mShadowMapShader.setViewProjectionMatrices({ matrices.begin(), matrices.end() });
                        
            for (ID meshInstanceID : mScene->meshInstances()) {
                auto& instance = mScene->meshInstances()[meshInstanceID];
                auto& subMeshes = ResourcePool::shared().meshes[instance.meshID()].subMeshes();
                
                mShadowMapShader.setModelMatrix(instance.transformation().modelMatrix());
                
                for (ID subMeshID : subMeshes) {
                    auto& subMesh = subMeshes[subMeshID];
                    subMesh.drawInstanced(6); // 6 for 6 cubemap faces
                }
            }
        }
    }
    
#pragma mark - Rendering
    
    void ShadowMapper::render() {
        ResourcePool::shared().VAO().bind();
        mShadowCascades = mScene->directionalLight().cascadesForBoundingBox(mScene->boundingBox(), mCascadeCount);

        renderOmnidirectionalShadowMaps();
        renderDirectionalShadowMaps();
        renderDirectionalPenumbra();
        renderOmnidirectionalPenumbras();
    }

}
