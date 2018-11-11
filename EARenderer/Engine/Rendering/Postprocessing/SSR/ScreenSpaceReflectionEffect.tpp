//
//  ScreenSpaceReflectionEffect.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 15.07.2018.
//  Copyright © 2018 MPO. All rights reserved.
//

#include "Drawable.hpp"

namespace EARenderer {

#pragma mark - Lifecycle

    template <GLTexture::Float TextureFormat>
    ScreenSpaceReflectionEffect<TextureFormat>::ScreenSpaceReflectionEffect(std::shared_ptr<GLFramebuffer> sharedFramebuffer,
                                                                            std::shared_ptr<PostprocessTexturePool<TextureFormat>> sharedTexturePool)
    :
    PostprocessEffect<TextureFormat>(sharedFramebuffer, sharedTexturePool),
    mBlurEffect(sharedFramebuffer, sharedTexturePool)
    { }

#pragma mark - Pivate Helpers

    template <GLTexture::Float TextureFormat>
    void ScreenSpaceReflectionEffect<TextureFormat>::traceReflections(const Camera& camera,
                                                                      std::shared_ptr<const SceneGBuffer> GBuffer,
                                                                      std::shared_ptr<typename PostprocessTexturePool<TextureFormat>::PostprocessTexture> rayHitInfo)
    {
        mSSRShader.bind();
        mSSRShader.ensureSamplerValidity([&]() {
            mSSRShader.setCamera(camera);
            mSSRShader.setGBuffer(*GBuffer);
        });

        this->mFramebuffer->redirectRenderingToTextures(GLFramebuffer::UnderlyingBuffer::None, rayHitInfo);

        Drawable::TriangleStripQuad::Draw();
    }

    template <GLTexture::Float TextureFormat>
    void ScreenSpaceReflectionEffect<TextureFormat>::blurProgressively(std::shared_ptr<typename PostprocessTexturePool<TextureFormat>::PostprocessTexture> mirrorReflections) {
        // Shape up the Gaussian curve to obtain [0.474, 0.233, 0.028, 0.001] weights
        // GPU Pro 5, 4.5.4 Pre-convolution Pass
        size_t blurRadius = 3;
        float sigma = 0.84;

        mirrorReflections->generateMipMaps();

        for (size_t mipLevel = 0; mipLevel < mirrorReflections->mipMapCount(); mipLevel++) {
            GaussianBlurSettings blurSettings { blurRadius, sigma, mipLevel, mipLevel + 1 };
            mBlurEffect.blur(mirrorReflections, mirrorReflections, blurSettings);
        }
    }

    template <GLTexture::Float TextureFormat>
    void ScreenSpaceReflectionEffect<TextureFormat>::traceCones(const Camera& camera,
                                                                std::shared_ptr<const typename PostprocessTexturePool<TextureFormat>::PostprocessTexture> reflections,
                                                                std::shared_ptr<const typename PostprocessTexturePool<TextureFormat>::PostprocessTexture> rayHitInfo,
                                                                std::shared_ptr<const SceneGBuffer> GBuffer,
                                                                std::shared_ptr<typename PostprocessTexturePool<TextureFormat>::PostprocessTexture> baseOutputImage,
                                                                std::shared_ptr<typename PostprocessTexturePool<TextureFormat>::PostprocessTexture> brightOutputImage)
    {
        mConeTracingShader.bind();
        mConeTracingShader.setCamera(camera);
        mConeTracingShader.ensureSamplerValidity([&]() {
            mConeTracingShader.setGBuffer(*GBuffer);
            mConeTracingShader.setRayHitInfo(*rayHitInfo);
            mConeTracingShader.setReflections(*reflections);
        });
        
        this->mFramebuffer->redirectRenderingToTextures(GLFramebuffer::UnderlyingBuffer::None, baseOutputImage, brightOutputImage);

        Drawable::TriangleStripQuad::Draw();
    }

#pragma mark - Public Interface

    template <GLTexture::Float TextureFormat>
    void ScreenSpaceReflectionEffect<TextureFormat>::applyReflections(const Camera& camera,
                                                                      std::shared_ptr<const SceneGBuffer> GBuffer,
                                                                      std::shared_ptr<typename PostprocessTexturePool<TextureFormat>::PostprocessTexture> lightBuffer,
                                                                      std::shared_ptr<typename PostprocessTexturePool<TextureFormat>::PostprocessTexture> baseOutputImage,
                                                                      std::shared_ptr<typename PostprocessTexturePool<TextureFormat>::PostprocessTexture> brightOutputImage)
    {
        auto rayTracingInfo = this->mTexturePool->claim();

        traceReflections(camera, GBuffer, rayTracingInfo);
        blurProgressively(lightBuffer);
        traceCones(camera, lightBuffer, rayTracingInfo, GBuffer, baseOutputImage, brightOutputImage);

        this->mTexturePool->putBack(rayTracingInfo);
    }

}
