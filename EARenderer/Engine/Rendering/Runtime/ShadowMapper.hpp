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
#include "GPUResourceController.hpp"

namespace EARenderer {

    class ShadowMapper {
    private:
        static constexpr uint8_t MaximumCascadeCount = 4;

        uint8_t mCascadeCount;

        const Scene *mScene;
        const SceneGBuffer *mGBuffer;
        const GPUResourceController *mGPUResourceController;
        const SharedResourceStorage *mResourceStorage;

        FrustumCascades mShadowCascades;
        RenderingSettings mSettings;

        GLSLShadowMap mShadowMapShader;
        GLSLDirectionalPenumbra mDirectionalPenumbraGenerationShader;
        GLSLOmnidirectionalPenumbra mOmnidirectionalPenumbraGenerationShader;

        GLFramebuffer mShadowFramebuffer;
        GLFramebuffer mOmnidirectionalShadowFramebuffer;
        GLFramebuffer mPenumbraFramebuffer;

        GLDepthTexture2DArray mDirectionalShadowMapArray;
        GLFloatTexture2D<GLTexture::Float::R16F> mDirectionalPenumbra;
        std::unordered_map<ID, GLDepthTextureCubemap> mOmnidirectionalShadowMaps;
        std::unordered_map<ID, GLFloatTexture2D<GLTexture::Float::R16F>> mOmnidirectionalPenumbras;

        GLSampler mBilinearSampler;

        void renderDirectionalPenumbra();

        void renderOmnidirectionalPenumbras();

        void renderDirectionalShadowMaps();

        void renderOmnidirectionalShadowMaps();

    public:
        ShadowMapper(
                const Scene *scene,
                const SharedResourceStorage *resourceStorage,
                const GPUResourceController *gpuResourceController,
                const SceneGBuffer *gBuffer,
                uint8_t cascadeCount
        );

        void setRenderingSettings(const RenderingSettings &settings);

        const GLDepthTexture2DArray &directionalShadowMapArray() const;

        const GLFloatTexture2D<GLTexture::Float::R16F> &directionalPenumbra() const;

        const GLDepthTextureCubemap &shadowMapForPointLight(ID pointLightID) const;

        const GLFloatTexture2D<GLTexture::Float::R16F> &penumbraForPointLight(ID pointLightID) const;

        const FrustumCascades &cascades() const;

        void render();
    };

}

#endif /* ShadowMapper_hpp */
