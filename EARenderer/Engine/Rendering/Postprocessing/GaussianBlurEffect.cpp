//
//  GaussianBlurEffect.cpp
//  EARenderer
//
//  Created by Pavel Muratov on 6/14/18.
//  Copyright © 2018 MPO. All rights reserved.
//

#include "GaussianBlurEffect.hpp"
#include "GaussianFunction.hpp"

#include <stdexcept>

namespace EARenderer {

#pragma mark - Lifecycle

    GaussianBlurEffect::GaussianBlurEffect() { }

#pragma mark - Getters


#pragma mark - Blur

    void GaussianBlurEffect::computeWeightsAndOffsets() {
        auto weights = GaussianFunction::Produce1DKernel(mSettings.radius, mSettings.sigma);

        mWeights.clear();
        mTextureOffsets.clear();

        // Kernel's center
        mWeights.push_back(weights[0]);
        mTextureOffsets.push_back(0.0);

        // Calculate texture offsets and combined weights to make advantage of hardware interpolation
        for (size_t i = 1; i <= mSettings.radius; i += 2) {
            float weight1 = weights[i];
            float weight2 = weights[i + 1];
            float totalWeight = weight1 + weight2;

            float texOffset1 = i;
            float texOffset2 = i + 1;

            float texOffset = (texOffset1 * weight1 + texOffset2 * weight2) / totalWeight;

            mWeights.push_back(totalWeight);
            mTextureOffsets.push_back(texOffset);
        }
    }

    void GaussianBlurEffect::blur(std::shared_ptr<const GLFloatTexture2D<GLTexture::Float::RGBA16F>> inputImage,
                                  std::shared_ptr<GLFloatTexture2D<GLTexture::Float::RGBA16F>> outputImage,
                                  std::shared_ptr<PostprocessTexturePool> texturePool,
                                  const GaussianBlurSettings& settings)
    {
        if (settings.radius == 0) throw std::invalid_argument("Blur radius must be greater than 0");

        if (settings != mSettings) {
            bool isOdd = settings.radius % 2 == 1;
            mSettings.radius = isOdd ? settings.radius + 1 : settings.radius;
            mSettings.sigma = settings.sigma;
            computeWeightsAndOffsets();
        }

        auto intermediateTexture = texturePool->claim();

        mBlurShader.bind();
        mBlurShader.setKernelWeights(mWeights);
        mBlurShader.setTextureOffsets(mTextureOffsets);
        mBlurShader.ensureSamplerValidity([&]() {
            mBlurShader.setTexture(*inputImage, settings.inputImageMipLevel);
        });

        mBlurShader.setBlurDirection(GLSLGaussianBlur::BlurDirection::Horizontal);

        texturePool->redirectRenderingToTextureMip(intermediateTexture, settings.outputImageMipLevel);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        mBlurShader.setBlurDirection(GLSLGaussianBlur::BlurDirection::Vertical);

        mBlurShader.ensureSamplerValidity([&]() {
            mBlurShader.setTexture(*intermediateTexture, settings.inputImageMipLevel);
        });

        texturePool->redirectRenderingToTextureMip(outputImage, settings.outputImageMipLevel);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        texturePool->putBack(intermediateTexture);
    }

}
