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

    ScreenSpaceReflectionEffect::ScreenSpaceReflectionEffect(std::shared_ptr<GLFramebuffer> sharedFramebuffer,
                                                             std::shared_ptr<PostprocessTexturePool> sharedTexturePool)
    :
    PostprocessEffect(sharedFramebuffer, sharedTexturePool),
    mBlurEffect(sharedFramebuffer, sharedTexturePool)
    { }

#pragma mark - Pivate Helpers

    void ScreenSpaceReflectionEffect::traceReflections(const Camera& camera,
                                                       std::shared_ptr<const SceneGBuffer> GBuffer,
                                                       std::shared_ptr<PostprocessTexturePool::PostprocessTexture> rayHitInfo)
    {
        mSSRShader.bind();
        mSSRShader.ensureSamplerValidity([&]() {
            mSSRShader.setCamera(camera);
            mSSRShader.setGBuffer(*GBuffer);
        });

        mFramebuffer->redirectRenderingToTexturesMip(0, rayHitInfo);

        TriangleStripQuad::Draw();
    }

    void ScreenSpaceReflectionEffect::blurProgressively(std::shared_ptr<PostprocessTexturePool::PostprocessTexture> mirrorReflections) {
        // Shape up the Gaussian curve to obtain [0.474, 0.233, 0.028, 0.001] weights
        // GPU Pro 5, 4.5.4 Pre-convolution Pass
        size_t blurRadius = 3;
        float sigma = 0.84;

        for (size_t mipLevel = 0; mipLevel < mirrorReflections->mipMapCount(); mipLevel++) {
            GaussianBlurSettings blurSettings { blurRadius, sigma, mipLevel, mipLevel + 1 };
            mBlurEffect.blur(mirrorReflections, mirrorReflections, blurSettings);
        }
    }

    void ScreenSpaceReflectionEffect::traceCones(std::shared_ptr<const PostprocessTexturePool::PostprocessTexture> reflections,
                                                 std::shared_ptr<const PostprocessTexturePool::PostprocessTexture> rayHitInfo,
                                                 std::shared_ptr<const SceneGBuffer> GBuffer,
                                                 std::shared_ptr<PostprocessTexturePool::PostprocessTexture> outputImage)
    {
        mConeTracingShader.bind();
        mConeTracingShader.ensureSamplerValidity([&]() {
            mConeTracingShader.setGBuffer(*GBuffer);
            mConeTracingShader.setRayHitInfo(*rayHitInfo);
            mConeTracingShader.setReflections(*reflections);
        });

        mFramebuffer->redirectRenderingToTexturesMip(0, outputImage);

        TriangleStripQuad::Draw();
    }

#pragma mark - Public Interface

    void ScreenSpaceReflectionEffect::applyReflections(const Camera& camera,
                                                       std::shared_ptr<const SceneGBuffer> GBuffer,
                                                       std::shared_ptr<PostprocessTexturePool::PostprocessTexture> lightBuffer,
                                                       std::shared_ptr<PostprocessTexturePool::PostprocessTexture> outputImage)
    {
        auto rayTracingInfo = mTexturePool->claim();

        traceReflections(camera, GBuffer, rayTracingInfo);
        blurProgressively(lightBuffer);
        traceCones(lightBuffer, rayTracingInfo, GBuffer, outputImage);

        mTexturePool->putBack(rayTracingInfo);
    }

}
