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
#include "GLTexture2DArray.hpp"
#include "GaussianBlurEffect.hpp"
#include "RenderingSettings.hpp"
#include "SceneGBuffer.hpp"
#include "GLSLDirectionalShadowMap.hpp"
#include "GLSLDirectionalExponentialShadowMap.hpp"
#include "GLSLOmnidirectionalExponentialShadowMap.hpp"

#include <memory>
#include <unordered_map>

namespace EARenderer {

    class ShadowMapper {
    private:
        static constexpr uint8_t MaximumCascadeCount = 4;

        uint8_t mCascadeCount;
    
        GLSLDirectionalShadowMap mDirectionalShadowMapShader;
        
        std::shared_ptr<GLFramebuffer> mFramebuffer;
        std::shared_ptr<GLDepthTexture2DArray> mDirectionalShadowMapArray;
        
        std::unordered_map<ID, size_t> mPointLightIDToArrayIndexMap;

        FrustumCascades mShadowCascades;
        RenderingSettings mSettings;

        const Scene *mScene = nullptr;

        void renderDirectionalShadowMaps();
        void renderOmnidirectionalShadowMaps();

    public:
        ShadowMapper(const Scene *scene, uint8_t cascadeCount);

        void setRenderingSettings(const RenderingSettings& settings);
        
        std::shared_ptr<const GLDepthTexture2DArray> directionalShadowMapArray() const;
        
        const FrustumCascades& cascades() const;
        
        size_t shadowMapIndexForPointLight(ID pointLightID) const;

        void render();
    };

}

#endif /* ShadowMapper_hpp */
