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
#include "GLSLDirectionalShadowMap.hpp"
#include "GLSLPenumbra.hpp"

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
    
        GLSLDirectionalShadowMap mDirectionalShadowMapShader;
        GLSLPenumbra mPenumbraGenerationShader;
        
        std::shared_ptr<GLFramebuffer> mShadowFramebuffer;
        std::shared_ptr<GLFramebuffer> mPenumbraFramebuffer;
        std::shared_ptr<GLDepthTexture2DArray> mDirectionalShadowMapArray;
        std::shared_ptr<GLFloatTexture2D<GLTexture::Float::R16F>> mDirectionalPenumbra;
        
        GLSampler mBilinearSampler;
        
        std::unordered_map<ID, size_t> mPointLightIDToArrayIndexMap;

        void renderDirectionalPenumbra();
        void renderDirectionalShadowMaps();
        void renderOmnidirectionalShadowMaps();

    public:
        ShadowMapper(const Scene *scene, std::shared_ptr<const SceneGBuffer> gBuffer, uint8_t cascadeCount);

        void setRenderingSettings(const RenderingSettings& settings);
        
        std::shared_ptr<const GLDepthTexture2DArray> directionalShadowMapArray() const;
        std::shared_ptr<const GLFloatTexture2D<GLTexture::Float::R16F>> directionalPenumbra() const;
        
        const FrustumCascades& cascades() const;
        
        size_t shadowMapIndexForPointLight(ID pointLightID) const;

        void render();
    };

}

#endif /* ShadowMapper_hpp */
