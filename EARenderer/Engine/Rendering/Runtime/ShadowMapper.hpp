//
//  ShadowMapper.hpp
//  EARenderer
//
//  Created by Pavel Muratov on 8/29/18.
//  Copyright © 2018 MPO. All rights reserved.
//

#ifndef ShadowMapper_hpp
#define ShadowMapper_hpp

#include "Scene.hpp"
#include "GLFramebuffer.hpp"
#include "GLTexture2D.hpp"
#include "GLTextureCubemapArray.hpp"
#include "GaussianBlurEffect.hpp"
#include "RenderingSettings.hpp"
#include "SceneGBuffer.hpp"
#include "GLSLDirectionalExponentialShadowMap.hpp"
#include "GLSLOmnidirectionalExponentialShadowMap.hpp"

#include <memory>
#include <unordered_map>

namespace EARenderer {

    class ShadowMapper {
    private:
        GLSLDirectionalExponentialShadowMap mDirectionalSMShader;
        GLSLDirectionalExponentialShadowMap mOmnidirectionalSMShader;

        std::shared_ptr<GLFloatTexture2D<GLTexture::Float::RGBA32F>> mDirectionalShadowMap;
        std::shared_ptr<GLFloatTextureCubemapArray<GLTexture::Float::R32F>> mOmnidirectionalShadowMaps;
        std::unordered_map<ID, size_t> mPointLightIDToArrayIndexMap;

        FrustumCascades mShadowCascades;
        GLDepthRenderbuffer mDepthRenderbuffer;
        std::shared_ptr<GLFramebuffer> mFramebuffer;
        std::shared_ptr<PostprocessTexturePool<GLTexture::Float::RGBA32F>> mTexturePool;
        GaussianBlurEffect<GLTexture::Float::RGBA32F> mBlurEffect;

        RenderingSettings mSettings;

        const Scene *mScene = nullptr;

        void renderDirectionalShadowMaps();
        void renderOmnidirectionalShadowMaps();

    public:
        ShadowMapper(const Scene *scene);

        void setRenderingSettings(const RenderingSettings& settings);

        const FrustumCascades& cascades() const;
        size_t shadowMapIndexForPointLight(ID pointLightID) const;
        std::shared_ptr<const GLFloatTexture2D<GLTexture::Float::RGBA32F>> directionalShadowMap() const;
        std::shared_ptr<const GLFloatTextureCubemapArray<GLTexture::Float::R32F>> omnidirectionalShadowMaps() const;

        void render();
    };

}

#endif /* ShadowMapper_hpp */
