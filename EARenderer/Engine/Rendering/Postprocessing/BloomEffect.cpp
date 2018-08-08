//
//  BloomFilter.cpp
//  EARenderer
//
//  Created by Pavel Muratov on 6/26/18.
//  Copyright © 2018 MPO. All rights reserved.
//

#include "BloomEffect.hpp"

namespace EARenderer {

#pragma mark - 

    void BloomEffect::bloom(std::shared_ptr<const PostprocessTexturePool::PostprocessTexture> baseImage,
                            std::shared_ptr<PostprocessTexturePool::PostprocessTexture> thresholdFilteredImage,
                            std::shared_ptr<PostprocessTexturePool::PostprocessTexture> outputImage,
                            std::shared_ptr<PostprocessTexturePool> texturePool,
                            const BloomSettings& settings)
    {
        thresholdFilteredImage->generateMipMaps();

        auto blurTexture = texturePool->claim();

        mSmallBlurEffect.blur(thresholdFilteredImage, blurTexture, texturePool, settings.smallBlurSettings);
        mMediumBlurEffect.blur(thresholdFilteredImage, blurTexture, texturePool, settings.mediumBlurSettings);
        mLargeBlurEffect.blur(thresholdFilteredImage, blurTexture, texturePool, settings.largeBlurSettings);

        float totalWeight = settings.smallBlurWeight + settings.mediumBlurWeight + settings.largeBlurWeight;
        float smallBlurWeightNorm = settings.smallBlurWeight / totalWeight * settings.bloomStrength;
        float mediumBlurWeightNorm = settings.mediumBlurWeight / totalWeight * settings.bloomStrength;
        float largeBlurWeightNorm = settings.largeBlurWeight / totalWeight * settings.bloomStrength;

        mBloomShader.bind();
        mBloomShader.ensureSamplerValidity([&]() {
            mBloomShader.setTextures(*baseImage, *blurTexture);
            mBloomShader.setTextureWeights(smallBlurWeightNorm, mediumBlurWeightNorm, largeBlurWeightNorm);
        });

        texturePool->redirectRenderingToTexturesMip(0, outputImage);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        texturePool->putBack(blurTexture);
    }

}
