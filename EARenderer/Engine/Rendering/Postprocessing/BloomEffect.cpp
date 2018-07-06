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

    void BloomEffect::bloom(std::shared_ptr<const GLHDRTexture2D> baseImage,
                            std::shared_ptr<GLHDRTexture2D> thresholdFilteredImage,
                            std::shared_ptr<GLHDRTexture2D> outputImage,
                            std::shared_ptr<PostprocessTexturePool> texturePool,
                            const BloomSettings& settings)
    {
        thresholdFilteredImage->generateMipmaps(2);

        auto smallBlurTexture = texturePool->claim();
        auto mediumBlurTexture = texturePool->claim();
        auto largeBlurTexture = texturePool->claim();

        mSmallBlurEffect.blur(thresholdFilteredImage, smallBlurTexture, texturePool, settings.smallBlurSettings);
        mMediumBlurEffect.blur(thresholdFilteredImage, mediumBlurTexture, texturePool, settings.mediumBlurSettings);
        mLargeBlurEffect.blur(thresholdFilteredImage, largeBlurTexture, texturePool, settings.largeBlurSettings);

        float totalWeight = settings.smallBlurWeight + settings.mediumBlurWeight + settings.largeBlurWeight;
        float smallBlurWeightNorm = settings.smallBlurWeight / totalWeight * settings.bloomStrength;
        float mediumBlurWeightNorm = settings.mediumBlurWeight / totalWeight * settings.bloomStrength;
        float largeBlurWeightNorm = settings.largeBlurWeight / totalWeight * settings.bloomStrength;

        mBloomShader.bind();
        mBloomShader.ensureSamplerValidity([&]() {
            mBloomShader.setTextures(*baseImage, *smallBlurTexture, *mediumBlurTexture, *largeBlurTexture);
            mBloomShader.setTextureWeights(smallBlurWeightNorm, mediumBlurWeightNorm, largeBlurWeightNorm);
        });

        texturePool->redirectRenderingToTextures(outputImage);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        texturePool->putBack(smallBlurTexture);
        texturePool->putBack(mediumBlurTexture);
        texturePool->putBack(largeBlurTexture);
    }

}
