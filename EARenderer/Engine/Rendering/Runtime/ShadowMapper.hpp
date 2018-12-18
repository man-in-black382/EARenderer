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
#include "GLSampler.hpp"
#include "GLTexture2D.hpp"
#include "GLTextureCubemapArray.hpp"
#include "GLTexture2DArray.hpp"
#include "GaussianBlurEffect.hpp"
#include "RenderingSettings.hpp"
#include "SceneGBuffer.hpp"
#include "GLSLShadowMap.hpp"
#include "GLSLDirectionalPenumbra.hpp"
#include "GLSLOmnidirectionalPenumbra.hpp"

#include <memory>
#include <unordered_map>

namespace EARenderer {

    class ShadowMapper {
    private:
        static constexpr uint8_t MaximumCascadeCount = 4;

        const Scene *mScene = nullptr;
        uint8_t mCascadeCount;
        std::shared_ptr<const SceneGBuffer> mGBuffer;

        FrustumCascades mShadowCascades;
        RenderingSettings mSettings;

        GLSLShadowMap mShadowMapShader;
        GLSLDirectionalPenumbra mDirectionalPenumbraGenerationShader;
        GLSLOmnidirectionalPenumbra mOmnidirectionalPenumbraGenerationShader;

        GLFramebuffer mShadowFramebuffer;
        GLFramebuffer mOmnidirectionalShadowFramebuffer;
        GLFramebuffer mPenumbraFramebuffer;

        GLDepthTexture2DArray mDirectionalShadowMapArray;
        GLNormalizedTexture2D<GLTexture::Normalized::R> mDirectionalPenumbra;
        std::unordered_map<ID, GLDepthTextureCubemap> mOmnidirectionalShadowMaps;
        std::unordered_map<ID, GLNormalizedTexture2D<GLTexture::Normalized::R>> mOmnidirectionalPenumbras;

        GLSampler mBilinearSampler;

        void renderDirectionalPenumbra();

        void renderOmnidirectionalPenumbras();

        void renderDirectionalShadowMaps();

        void renderOmnidirectionalShadowMaps();

    public:
        ShadowMapper(const Scene *scene, std::shared_ptr<const SceneGBuffer> gBuffer, uint8_t cascadeCount);

        void setRenderingSettings(const RenderingSettings &settings);

        const GLDepthTexture2DArray &directionalShadowMapArray() const;

        const GLNormalizedTexture2D<GLTexture::Normalized::R> &directionalPenumbra() const;

        const GLDepthTextureCubemap &shadowMapForPointLight(ID pointLightID) const;

        const GLNormalizedTexture2D<GLTexture::Normalized::R> &penumbraForPointLight(ID pointLightID) const;

        const FrustumCascades &cascades() const;

        void render();
    };

}

#endif /* ShadowMapper_hpp */
