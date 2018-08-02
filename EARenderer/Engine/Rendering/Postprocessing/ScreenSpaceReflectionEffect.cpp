//
//  ScreenSpaceReflectionEffect.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 15.07.2018.
//  Copyright © 2018 MPO. All rights reserved.
//

#include "ScreenSpaceReflectionEffect.hpp"

namespace EARenderer {

#pragma mark - Pivate Helpers

    void ScreenSpaceReflectionEffect::traceReflections(const Camera& camera,
                                                       std::shared_ptr<const PostprocessTexturePool::PostprocessTexture> lightBuffer,
                                                       std::shared_ptr<const SceneGBuffer> GBuffer,
                                                       std::shared_ptr<PostprocessTexturePool::PostprocessTexture> mirrorReflections,
                                                       std::shared_ptr<PostprocessTexturePool::PostprocessTexture> rayHitInfo,
                                                       std::shared_ptr<PostprocessTexturePool> texturePool)
    {
        mSSRShader.bind();
        mSSRShader.ensureSamplerValidity([&]() {
            mSSRShader.setCamera(camera);
            mSSRShader.setFrame(*lightBuffer);
            mSSRShader.setGBuffer(*GBuffer);
        });

        texturePool->redirectRenderingToTexturesMip(0, mirrorReflections, rayHitInfo);

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }

    void ScreenSpaceReflectionEffect::blurReflections(std::shared_ptr<PostprocessTexturePool::PostprocessTexture> mirrorReflections,
                                                      std::shared_ptr<PostprocessTexturePool> texturePool)
    {
        // Shape up the Gaussian curve to obtain [0.474, 0.233, 0.028, 0.001] weights
        // GPU Pro 5, 4.5.4 Pre-convolution Pass
        size_t blurRadius = 3;
        float sigma = 0.84;

        for (size_t mipLevel = 0; mipLevel < mirrorReflections->mipMapsCount(); mipLevel++) {
            GaussianBlurSettings blurSettings { blurRadius, sigma, mipLevel, mipLevel + 1 };
            mBlurEffect.blur(mirrorReflections, mirrorReflections, texturePool, blurSettings);
        }
    }

    void ScreenSpaceReflectionEffect::traceCones(std::shared_ptr<const PostprocessTexturePool::PostprocessTexture> reflections,
                                                 std::shared_ptr<const PostprocessTexturePool::PostprocessTexture> rayHitInfo,
                                                 std::shared_ptr<const SceneGBuffer> GBuffer,
                                                 std::shared_ptr<PostprocessTexturePool::PostprocessTexture> outputImage,
                                                 std::shared_ptr<PostprocessTexturePool> texturePool)
    {
        mConeTracingShader.bind();
        mConeTracingShader.ensureSamplerValidity([&]() {
            mConeTracingShader.setGBuffer(*GBuffer);
            mConeTracingShader.setRayHitInfo(*rayHitInfo);
            mConeTracingShader.setReflections(*reflections);
        });

        texturePool->redirectRenderingToTexturesMip(0, outputImage);

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }

#pragma mark - Public Interface

    void ScreenSpaceReflectionEffect::applyReflections(const Camera& camera,
                                                       std::shared_ptr<const PostprocessTexturePool::PostprocessTexture> lightBuffer,
                                                       std::shared_ptr<const SceneGBuffer> GBuffer,
                                                       std::shared_ptr<PostprocessTexturePool::PostprocessTexture> outputImage,
                                                       std::shared_ptr<PostprocessTexturePool> texturePool)
    {
        auto mirrorReflections = texturePool->claim();
        auto rayTracingInfo = texturePool->claim();

        traceReflections(camera, lightBuffer, GBuffer, mirrorReflections, rayTracingInfo, texturePool);
        blurReflections(mirrorReflections, texturePool);
        traceCones(mirrorReflections, rayTracingInfo, GBuffer, outputImage, texturePool);

        texturePool->putBack(mirrorReflections);
        texturePool->putBack(rayTracingInfo);
    }

}
