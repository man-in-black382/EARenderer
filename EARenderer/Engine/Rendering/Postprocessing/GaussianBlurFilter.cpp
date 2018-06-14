//
//  GaussianBlurEffect.cpp
//  EARenderer
//
//  Created by Pavel Muratov on 6/14/18.
//  Copyright © 2018 MPO. All rights reserved.
//

#include "GaussianBlurFilter.hpp"
#include "GaussianFunction.hpp"

namespace EARenderer {

#pragma mark - Lifecycle

    GaussianBlurFilter::GaussianBlurFilter(std::shared_ptr<const GLHDRTexture2D> inputImage)
    :
    mInputImage(inputImage),
    mFirstOutputImage(std::make_shared<GLHDRTexture2D>(inputImage->size())),
    mSecondOutputImage(std::make_shared<GLHDRTexture2D>(inputImage->size())),
    mFirstFramebuffer(inputImage->size()),
    mSecondFramebuffer(inputImage->size())
    {
        mFirstFramebuffer.attachTexture(*mFirstOutputImage);
        mSecondFramebuffer.attachTexture(*mSecondOutputImage);

        auto offsets = textureOffsets();
    }

#pragma mark - Getters

    std::shared_ptr<GLHDRTexture2D> GaussianBlurFilter::outputImage() const {
        return mFirstOutputImage;
    }

#pragma mark - Blur

    std::vector<float> GaussianBlurFilter::textureOffsets() const {
        auto kernel = GaussianFunction::Produce1DKernel(3, 1.0);
        std::vector<float> halfKerhel(kernel.begin() + kernel.size() / 2, kernel.end());
        return halfKerhel;
    }

    std::shared_ptr<GLHDRTexture2D> GaussianBlurFilter::blur(size_t radius) {
        return mSecondOutputImage;
    }

}
