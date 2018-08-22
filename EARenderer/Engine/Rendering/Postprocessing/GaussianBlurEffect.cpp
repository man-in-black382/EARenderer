//
//  GaussianBlurEffect.cpp
//  EARenderer
//
//  Created by Pavel Muratov on 6/14/18.
//  Copyright © 2018 MPO. All rights reserved.
//

#include "GaussianBlurEffect.hpp"
#include "GaussianFunction.hpp"
#include "Drawable.hpp"

#include <stdexcept>

namespace EARenderer {

#pragma mark - Blur

    void GaussianBlurEffect::computeWeightsAndOffsetsIfNeeded(const GaussianBlurSettings& settings) {
        if (settings == mSettings && !mWeights.empty()) {
            return;
        }

        bool isOdd = settings.radius % 2 == 1;
        mSettings.radius = isOdd ? settings.radius + 1 : settings.radius;
        mSettings.sigma = settings.sigma;

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

    void GaussianBlurEffect::blur(std::shared_ptr<const PostprocessTexturePool::PostprocessTexture> inputImage,
                                  std::shared_ptr<PostprocessTexturePool::PostprocessTexture> outputImage,
                                  const GaussianBlurSettings& settings)
    {
        if (settings.radius == 0) throw std::invalid_argument("Blur radius must be greater than 0");

        computeWeightsAndOffsetsIfNeeded(settings);

        auto intermediateTexture = mTexturePool->claim();

        mBlurShader.bind();
        mBlurShader.setRenderTargetSize(inputImage->mipMapSize(settings.outputImageMipLevel));
        mBlurShader.setKernelWeights(mWeights);
        mBlurShader.setTextureOffsets(mTextureOffsets);
        mBlurShader.ensureSamplerValidity([&]() {
            mBlurShader.setTexture(*inputImage, settings.inputImageMipLevel);
        });

        // For the first pass we read from input mip level and output
        // to output mip level of the intermediate texture
        //
        mBlurShader.setBlurDirection(GLSLGaussianBlur::BlurDirection::Horizontal);

        mFramebuffer->redirectRenderingToTexturesMip(settings.outputImageMipLevel, intermediateTexture);
        TriangleStripQuad::Draw();

        // But, in the second pass, we read and write from and to the same
        // mip level of intermediate and output textures
        //
        mBlurShader.setBlurDirection(GLSLGaussianBlur::BlurDirection::Vertical);

        mBlurShader.ensureSamplerValidity([&]() {
            mBlurShader.setTexture(*intermediateTexture, settings.outputImageMipLevel);
        });

        mFramebuffer->redirectRenderingToTexturesMip(settings.outputImageMipLevel, outputImage);
        TriangleStripQuad::Draw();

        mTexturePool->putBack(intermediateTexture);
    }

}
