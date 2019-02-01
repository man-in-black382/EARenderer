//
//  ScreenSpaceReflectionEffect.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 15.07.2018.
//  Copyright © 2018 MPO. All rights reserved.
//

#include "ScreenSpaceReflectionEffect.hpp"
#include "Drawable.hpp"

namespace EARenderer {

#pragma mark - Lifecycle

    ScreenSpaceReflectionEffect::ScreenSpaceReflectionEffect(GLFramebuffer *sharedFramebuffer, PostprocessTexturePool *sharedTexturePool)
            : PostprocessEffect(sharedFramebuffer, sharedTexturePool), mBlurEffect(sharedFramebuffer, sharedTexturePool) {}

#pragma mark - Pivate Helpers

    void ScreenSpaceReflectionEffect::traceReflections(const Camera &camera, const SceneGBuffer &GBuffer, PostprocessTexturePool::PostprocessTexture &rayHitInfo) {
        mSSRShader.bind();
        mSSRShader.ensureSamplerValidity([&]() {
            mSSRShader.setCamera(camera);
            mSSRShader.setGBuffer(GBuffer);
        });

        mFramebuffer->redirectRenderingToTextures(GLFramebuffer::UnderlyingBuffer::None, &rayHitInfo);

        Drawable::TriangleStripQuad::Draw();
    }

    void ScreenSpaceReflectionEffect::blurProgressively(PostprocessTexturePool::PostprocessTexture &mirrorReflections) {
        // Shape up the Gaussian curve to obtain [0.474, 0.233, 0.028, 0.001] weights
        // GPU Pro 5, 4.5.4 Pre-convolution Pass
        size_t blurRadius = 3;
        float sigma = 0.84;

        mirrorReflections.generateMipMaps();

        for (size_t mipLevel = 0; mipLevel < mirrorReflections.mipMapCount(); mipLevel++) {
            GaussianBlurSettings blurSettings{blurRadius, sigma, mipLevel, mipLevel + 1};
            mBlurEffect.blur(mirrorReflections, mirrorReflections, blurSettings);
        }
    }

    void ScreenSpaceReflectionEffect::traceCones(
            const Camera &camera,
            const PostprocessTexturePool::PostprocessTexture &lightBuffer,
            const PostprocessTexturePool::PostprocessTexture &rayHitInfo,
            const SceneGBuffer &GBuffer,
            const ImageBasedLightProbe *IBLProbe,
            PostprocessTexturePool::PostprocessTexture &baseOutputImage,
            PostprocessTexturePool::PostprocessTexture &brightOutputImage) {

        mConeTracingShader.bind();
        mConeTracingShader.setCamera(camera);
        mConeTracingShader.ensureSamplerValidity([&]() {
            mConeTracingShader.setGBuffer(GBuffer);
            mConeTracingShader.setRayHitInfo(rayHitInfo);
            mConeTracingShader.setReflections(lightBuffer);
            if (IBLProbe) mConeTracingShader.setIBLProbe(*IBLProbe);
        });

        mFramebuffer->redirectRenderingToTextures(GLFramebuffer::UnderlyingBuffer::None, &baseOutputImage, &brightOutputImage);
        Drawable::TriangleStripQuad::Draw();
    }

#pragma mark - Public Interface

    void ScreenSpaceReflectionEffect::applyReflections(
            const Camera &camera,
            const SceneGBuffer &GBuffer,
            const ImageBasedLightProbe *IBLProbe,
            PostprocessTexturePool::PostprocessTexture &lightBuffer,
            PostprocessTexturePool::PostprocessTexture &baseOutputImage,
            PostprocessTexturePool::PostprocessTexture &brightOutputImage) {

        auto rayTracingInfo = mTexturePool->claim();

        traceReflections(camera, GBuffer, *rayTracingInfo);
        blurProgressively(lightBuffer);
        traceCones(camera, lightBuffer, *rayTracingInfo, GBuffer, IBLProbe, baseOutputImage, brightOutputImage);

        mTexturePool->putBack(rayTracingInfo);
    }

}
