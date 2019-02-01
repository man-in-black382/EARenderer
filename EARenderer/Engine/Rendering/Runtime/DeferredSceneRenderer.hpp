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
        uint8_t mNumberOfIrradianceMips = 5;
        glm::ivec3 mProbeGridResolution;

        const Scene *mScene;
        const SharedResourceStorage *mResourceStorage;
        const GPUResourceController *mGPUResourceController;
        const DefaultRenderComponentsProviding *mDefaultRenderComponentsProvider;
        const SurfelData *mSurfelData;
        const DiffuseLightProbeData *mProbeData;
        const SceneGBuffer *mGBuffer;

        RenderingSettings mSettings;

        GLFramebuffer mFramebuffer;
        PostprocessTexturePool mPostprocessTexturePool;

        BloomEffect mBloomEffect;
        ToneMappingEffect mToneMappingEffect;
        ScreenSpaceReflectionEffect mSSREffect;
        SMAAEffect mSMAAEffect;

        ShadowMapper mShadowMapper;
        DirectLightAccumulator mDirectLightAccumulator;
        IndirectLightAccumulator mIndirectLightAccumulator;

        GLSLDepthPrepass mDepthPrepassShader;
        GLSLSkybox mSkyboxShader;
        GLSLFullScreenQuad mFSQuadShader;

        void bindDefaultFramebuffer();

        void renderSkybox();

        void renderFinalImage(const PostprocessTexturePool::PostprocessTexture& image);

    public:
        using DebugOpportunity = std::function<void()>;

        DeferredSceneRenderer(
                const Scene *scene,
                const SharedResourceStorage *resourceStorage,
                const GPUResourceController *gpuResourceController,
                const DefaultRenderComponentsProviding *provider,
                const SurfelData *surfelData,
                const DiffuseLightProbeData *diffuseProbeData,
                const SceneGBuffer *gBuffer,
                const RenderingSettings &settings
        );

        // Setters
        void setRenderingSettings(const RenderingSettings &settings);

        // Getters
        const std::array<GLLDRTexture3D, 4> &gridProbesSphericalHarmonics() const;

        const GLFloatTexture2D<GLTexture::Float::R16F> &surfelsLuminanceMap() const;

        const GLFloatTexture2D<GLTexture::Float::R16F> &surfelClustersLuminanceMap() const;

        /**
         Renders the scene

         @param debugClosure render any debug information needed inside this closure
         before the final frame is displayed
         */
        void render(const DebugOpportunity &debugClosure = [] {});
    };

}

#endif /* DeferredSceneRenderer_hpp */
