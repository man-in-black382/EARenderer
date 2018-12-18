//
//  DeferredSceneRenderer.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 30.06.2018.
//  Copyright © 2018 MPO. All rights reserved.
//

#ifndef DeferredSceneRenderer_hpp
#define DeferredSceneRenderer_hpp

#include <unordered_set>
#include <array>
#include <memory>
#include <functional>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include "Scene.hpp"
#include "SceneGBuffer.hpp"
#include "PostprocessTexturePool.hpp"
#include "GLFramebuffer.hpp"
#include "DefaultRenderComponentsProviding.hpp"
#include "FrustumCascades.hpp"
#include "Ray3D.hpp"
#include "SurfelData.hpp"
#include "ShadowMapper.hpp"
#include "DiffuseLightProbeData.hpp"
#include "RenderingSettings.hpp"
#include "GaussianBlurEffect.hpp"
#include "BloomEffect.hpp"
#include "ToneMappingEffect.hpp"
#include "ScreenSpaceReflectionEffect.hpp"
#include "SMAAEffect.hpp"
#include "DirectLightAccumulator.hpp"
#include "IndirectLightAccumulator.hpp"

#include "GLSLDepthPrepass.hpp"
#include "GLSLDirectLightEvaluation.hpp"
#include "GLSLFullScreenQuad.hpp"
#include "GLSLSkybox.hpp"

#include "GLTexture2DArray.hpp"
#include "GLTexture2DArray.hpp"
#include "GLLDRTexture3D.hpp"
#include "GLHDRTexture3D.hpp"
#include "GLBufferTexture.hpp"

namespace EARenderer {

    class DeferredSceneRenderer {
    private:
        using HalfPrecisionTexturePool = PostprocessTexturePool<GLTexture::Float::RGBA16F>;
        using HighPrecisionTexturePool = PostprocessTexturePool<GLTexture::Float::RGBA32F>;

        uint8_t mNumberOfIrradianceMips = 5;
        glm::ivec3 mProbeGridResolution;

        const Scene *mScene = nullptr;
        const DefaultRenderComponentsProviding *mDefaultRenderComponentsProvider = nullptr;

        RenderingSettings mSettings;

        std::shared_ptr<GLFramebuffer> mFramebuffer;
        std::shared_ptr<HalfPrecisionTexturePool> mPostprocessTexturePool;
        BloomEffect<GLTexture::Float::RGBA16F> mBloomEffect;
        ToneMappingEffect<GLTexture::Float::RGBA16F> mToneMappingEffect;
        ScreenSpaceReflectionEffect<GLTexture::Float::RGBA16F> mSSREffect;
        SMAAEffect<GLTexture::Float::RGBA16F> mSMAAEffect;

        std::shared_ptr<const SurfelData> mSurfelData;
        std::shared_ptr<const DiffuseLightProbeData> mProbeData;
        std::shared_ptr<const SceneGBuffer> mGBuffer;
        std::shared_ptr<ShadowMapper> mShadowMapper;
        std::shared_ptr<DirectLightAccumulator> mDirectLightAccumulator;
        std::shared_ptr<IndirectLightAccumulator> mIndirectLightAccumulator;

        GLSLDepthPrepass mDepthPrepassShader;
        GLSLSkybox mSkyboxShader;
        GLSLDirectLightEvaluation mCookTorranceShader;

        GLSLFullScreenQuad mFSQuadShader;

        void bindDefaultFramebuffer();

        void performDepthPrepass();

        void renderSkybox();

        void renderFinalImage(std::shared_ptr<HalfPrecisionTexturePool::PostprocessTexture> image);

    public:
        using DebugOpportunity = std::function<void()>;

        DeferredSceneRenderer(const Scene *scene,
                const DefaultRenderComponentsProviding *provider,
                const RenderingSettings &settings,
                std::shared_ptr<const SurfelData> surfelData,
                std::shared_ptr<const DiffuseLightProbeData> diffuseProbeData,
                std::shared_ptr<const SceneGBuffer> GBuffer);

        // Setters
        void setRenderingSettings(const RenderingSettings &settings);

        // Getters
        std::shared_ptr<const std::array<GLLDRTexture3D, 4>> gridProbesSphericalHarmonics() const;

        std::shared_ptr<const GLFloatTexture2D<GLTexture::Float::R16F>> surfelsLuminanceMap() const;

        std::shared_ptr<const GLFloatTexture2D<GLTexture::Float::R16F>> surfelClustersLuminanceMap() const;

        /**
         Renders the scene

         @param debugClosure render any debug information needed inside this closure
         before the final frame is displayed
         */
        void render(const DebugOpportunity &debugClosure = [] {
        });
    };

}

#endif /* DeferredSceneRenderer_hpp */
