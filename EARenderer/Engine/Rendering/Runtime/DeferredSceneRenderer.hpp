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
#include "DiffuseLightProbeData.hpp"
#include "RenderingSettings.hpp"
#include "GaussianBlurEffect.hpp"
#include "BloomEffect.hpp"
#include "ToneMappingEffect.hpp"
#include "ScreenSpaceReflectionEffect.hpp"

#include "GLSLDepthPrepass.hpp"
#include "GLSLDeferredCookTorrance.hpp"
#include "GLSLFullScreenQuad.hpp"
#include "GLSLSkybox.hpp"
#include "GLSLGenericGeometry.hpp"
#include "GLSLEquirectangularMapConversion.hpp"
#include "GLSLRadianceConvolution.hpp"
#include "GLSLBRDFIntegration.hpp"
#include "GLSLSurfelLighting.hpp"
#include "GLSLSurfelClusterAveraging.hpp"
#include "GLSLGridLightProbesUpdate.hpp"
#include "GLSLGridLightProbeRendering.hpp"
#include "GLSLLightProbeLinksRendering.hpp"
#include "GLSLProbeOcclusionRendering.hpp"
#include "GLSLDirectionalExponentialShadowMap.hpp"

#include "GLDepthTextureCubemap.hpp"
#include "GLHDRTexture2DArray.hpp"
#include "GLDepthTexture2DArray.hpp"

#include "GLHDRTextureCubemap.hpp"
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

        std::shared_ptr<const SurfelData> mSurfelData;
        std::shared_ptr<const DiffuseLightProbeData> mDiffuseProbeData;
        std::shared_ptr<const SceneGBuffer> mGBuffer;

        FrustumCascades mShadowCascades;
        std::shared_ptr<HighPrecisionTexturePool> mDirectionalShadowTexturePool;
        std::shared_ptr<GLFramebuffer> mDirectionalShadowFramebuffer;
        std::shared_ptr<GLFloatTexture2D<GLTexture::Float::RGBA32F>> mDirectionalExponentialShadowMap;
        GLDepthRenderbuffer mDirectionalShadowDepthRenderbuffer;
        GaussianBlurEffect<GLTexture::Float::RGBA32F> mShadowBlurEffect;

        GLSLDepthPrepass mDepthPrepassShader;
        GLSLDirectionalExponentialShadowMap mDirectionalESMShader;
        GLSLSkybox mSkyboxShader;
        GLSLDeferredCookTorrance mCookTorranceShader;
        GLSLSurfelLighting mSurfelLightingShader;
        GLSLSurfelClusterAveraging mSurfelClusterAveragingShader;
        GLSLGridLightProbesUpdate mGridProbesUpdateShader;

        GLFloatTexture2D<GLTexture::Float::RGBA16F> mSurfelsLuminanceMap;
        GLFloatTexture2D<GLTexture::Float::RGBA16F> mSurfelClustersLuminanceMap;
        GLFramebuffer mSurfelsLuminanceFramebuffer;
        GLFramebuffer mSurfelClustersLuminanceFramebuffer;

        std::array<GLLDRTexture3D, 4> mGridProbesSHMaps;
        GLFramebuffer mGridProbesSHFramebuffer;

        GLSLFullScreenQuad mFSQuadShader;

        std::shared_ptr<GLFramebuffer> mPostprocessFramebuffer;
        std::shared_ptr<HalfPrecisionTexturePool> mPostprocessTexturePool;
        BloomEffect<GLTexture::Float::RGBA16F> mBloomEffect;
        ToneMappingEffect<GLTexture::Float::RGBA16F> mToneMappingEffect;
        ScreenSpaceReflectionEffect<GLTexture::Float::RGBA16F> mSSREffect;

        std::shared_ptr<GLFloatTexture2D<GLTexture::Float::RGBA16F>> mFrame;
        std::shared_ptr<GLFloatTexture2D<GLTexture::Float::RGBA16F>> mPreviousFrame;
        std::shared_ptr<GLFloatTexture2D<GLTexture::Float::RGBA16F>> mThresholdFilteredOutputFrame;

        void setupGLState();
        void setupFramebuffers();

        void bindDefaultFramebuffer();
        void swapFrames();
        void performDepthPrepass();
        void renderShadowMaps();
        void relightSurfels();
        void averageSurfelClusterLuminances();
        void updateGridProbes();
        void renderMeshes();
        void renderSkybox();
        void renderFinalImage(const GLFloatTexture2D<GLTexture::Float::RGBA16F>& image);

    public:
        DeferredSceneRenderer(const Scene* scene,
                              const DefaultRenderComponentsProviding *provider,
                              const RenderingSettings& settings,
                              std::shared_ptr<const SurfelData> surfelData,
                              std::shared_ptr<const DiffuseLightProbeData> diffuseProbeData,
                              std::shared_ptr<const SceneGBuffer> GBuffer);

        const FrustumCascades& shadowCascades() const;

        void setRenderingSettings(const RenderingSettings& settings);

        void render();
    };

}

#endif /* DeferredSceneRenderer_hpp */
