//
//  BloomFilter.cpp
//  EARenderer
//
//  Created by Pavel Muratov on 6/26/18.
//  Copyright © 2018 MPO. All rights reserved.
//

#include "BloomEffect.hpp"
#include "Drawable.hpp"

namespace EARenderer {

#pragma mark - Lifecycle

    BloomEffect::BloomEffect(std::shared_ptr<GLFramebuffer> sharedFramebuffer,
                             std::shared_ptr<PostprocessTexturePool> sharedTexturePool)
    :
    EARenderer::PostprocessEffect(sharedFramebuffer, sharedTexturePool),
    mSmallBlurEffect(sharedFramebuffer, sharedTexturePool),
    mMediumBlurEffect(sharedFramebuffer, sharedTexturePool),
    mLargeBlurEffect(sharedFramebuffer, sharedTexturePool)
    { }

#pragma mark - Bloom

    void BloomEffect::bloom(std::shared_ptr<const PostprocessTexturePool::PostprocessTexture> baseImage,
                            std::shared_ptr<PostprocessTexturePool::PostprocessTexture> thresholdFilteredImage,
                            std::shared_ptr<PostprocessTexturePool::PostprocessTexture> outputImage,
                            const BloomSettings& settings)
    {
        thresholdFilteredImage->generateMipMaps();

        auto blurTexture = mTexturePool->claim();

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

        mFramebuffer->redirectRenderingToTexturesMip(0, outputImage);
        TriangleStripQuad::Draw();

        mTexturePool->putBack(blurTexture);
    }

}
