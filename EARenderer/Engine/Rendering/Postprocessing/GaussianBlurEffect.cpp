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

    GaussianBlurEffect::GaussianBlurEffect(std::shared_ptr<const GLHDRTexture2D> inputImage)
    :
    mInputImage(inputImage),
    mFirstOutputImage(std::make_shared<GLHDRTexture2D>(inputImage->size())),
    mSecondOutputImage(std::make_shared<GLHDRTexture2D>(inputImage->size())),
    mFirstFramebuffer(inputImage->size()),
    mSecondFramebuffer(inputImage->size())
    {
        mFirstFramebuffer.attachTexture(*mFirstOutputImage);
        mSecondFramebuffer.attachTexture(*mSecondOutputImage);
    }

#pragma mark - Getters

    std::shared_ptr<GLHDRTexture2D> GaussianBlurEffect::outputImage() const {
        return mSecondOutputImage;
    }

#pragma mark - Blur

    void GaussianBlurEffect::computeWeightsAndOffsets() {
        auto weights = GaussianFunction::Produce1DKernel(mBlurRadius, mStandardDeviation);

        mWeights.clear();
        mTextureOffsets.clear();

        // Kernel's center
        mWeights.push_back(weights[0]);
        mTextureOffsets.push_back(0.0);

        // Calculate texture offsets and combined weights to make advantage of hardware interpolation
        for (size_t i = 1; i <= mBlurRadius; i += 2) {
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

    std::shared_ptr<GLHDRTexture2D> GaussianBlurEffect::blur(size_t radius, size_t standardDeviation) {
        if (radius == 0) throw std::invalid_argument("Blur radius must be greater than 0");

        if (mBlurRadius != radius || mStandardDeviation != standardDeviation) {
            bool isOdd = radius % 2 == 1;
            mBlurRadius = isOdd ? radius + 1 : radius;
            mStandardDeviation = standardDeviation;
            computeWeightsAndOffsets();
        }

        mBlurShader.bind();
        mBlurShader.setKernelWeights(mWeights);
        mBlurShader.setTextureOffsets(mTextureOffsets);
        mBlurShader.ensureSamplerValidity([&]() {
            mBlurShader.setTexture(*mInputImage);
        });

        mBlurShader.setBlurDirection(GLSLGaussianBlur::BlurDirection::Horizontal);

        mFirstFramebuffer.bind();
        mFirstFramebuffer.viewport().apply();
        glClear(GL_COLOR_BUFFER_BIT);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        mBlurShader.setBlurDirection(GLSLGaussianBlur::BlurDirection::Vertical);

        mBlurShader.ensureSamplerValidity([&]() {
            mBlurShader.setTexture(*mFirstOutputImage);
        });

        mSecondFramebuffer.bind();
        mSecondFramebuffer.viewport().apply();
        glClear(GL_COLOR_BUFFER_BIT);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        return mSecondOutputImage;
    }

    std::shared_ptr<GLHDRTexture2D> GaussianBlurEffect::blur(size_t radius) {
        return blur(radius, radius / 2);
    }

}
