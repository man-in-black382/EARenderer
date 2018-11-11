//
//  BloomFilter.cpp
//  EARenderer
//
//  Created by Pavel Muratov on 6/26/18.
//  Copyright © 2018 MPO. All rights reserved.
//

#include "Drawable.hpp"

namespace EARenderer {

#pragma mark - Lifecycle

    template <GLTexture::Float TextureFormat>
    BloomEffect<TextureFormat>::BloomEffect(std::shared_ptr<GLFramebuffer> sharedFramebuffer,
                                            std::shared_ptr<PostprocessTexturePool<TextureFormat>> sharedTexturePool)
    :
    PostprocessEffect<TextureFormat>(sharedFramebuffer, sharedTexturePool),
    mSmallBlurEffect(sharedFramebuffer, sharedTexturePool),
    mMediumBlurEffect(sharedFramebuffer, sharedTexturePool),
    mLargeBlurEffect(sharedFramebuffer, sharedTexturePool)
    { }

#pragma mark - Bloom

    template <GLTexture::Float TextureFormat>
    void BloomEffect<TextureFormat>::bloom(std::shared_ptr<const typename PostprocessTexturePool<TextureFormat>::PostprocessTexture> baseImage,
                                           std::shared_ptr<typename PostprocessTexturePool<TextureFormat>::PostprocessTexture> thresholdFilteredImage,
                                           std::shared_ptr<typename PostprocessTexturePool<TextureFormat>::PostprocessTexture> outputImage,
                                           const BloomSettings& settings)
    {
        thresholdFilteredImage->generateMipMaps();

        auto blurTexture = this->mTexturePool->claim();

        mSmallBlurEffect.blur(thresholdFilteredImage, blurTexture, settings.smallBlurSettings);
        mMediumBlurEffect.blur(thresholdFilteredImage, blurTexture, settings.mediumBlurSettings);
        mLargeBlurEffect.blur(thresholdFilteredImage, blurTexture, settings.largeBlurSettings);

        float totalWeight = settings.smallBlurWeight + settings.mediumBlurWeight + settings.largeBlurWeight;
        float smallBlurWeightNorm = settings.smallBlurWeight / totalWeight * settings.bloomStrength;
        float mediumBlurWeightNorm = settings.mediumBlurWeight / totalWeight * settings.bloomStrength;
        float largeBlurWeightNorm = settings.largeBlurWeight / totalWeight * settings.bloomStrength;

        mBloomShader.bind();
        mBloomShader.ensureSamplerValidity([&]() {
            mBloomShader.setTextures(*baseImage, *blurTexture);
            mBloomShader.setTextureWeights(smallBlurWeightNorm, mediumBlurWeightNorm, largeBlurWeightNorm);
        });

        this->mFramebuffer->redirectRenderingToTextures(GLFramebuffer::UnderlyingBuffer::None, outputImage);
        Drawable::TriangleStripQuad::Draw();

        this->mTexturePool->putBack(blurTexture);
    }

}
