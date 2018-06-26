//
//  BloomFilter.cpp
//  EARenderer
//
//  Created by Pavel Muratov on 6/26/18.
//  Copyright © 2018 MPO. All rights reserved.
//

#include "BloomEffect.hpp"

namespace EARenderer {

    BloomEffect::BloomEffect(std::shared_ptr<const GLHDRTexture2D> baseImage,
                             std::shared_ptr<const GLHDRTexture2D> thresholdFilteredImage)
    :
    mBaseImage(baseImage),
    mOutputImage(std::make_shared<GLHDRTexture2D>(baseImage->size())),

    mLargeThresholdFilteredImage(std::make_shared<GLHDRTexture2D>(thresholdFilteredImage->size())),
    mMediumThresholdFilteredImage(std::make_shared<GLHDRTexture2D>(thresholdFilteredImage->size().transformedBy(glm::vec2(0.5)))),
    mSmallThresholdFilteredImage(std::make_shared<GLHDRTexture2D>(thresholdFilteredImage->size().transformedBy(glm::vec2(0.25)))),

    mLargeBlurEffect(mSmallThresholdFilteredImage),
    mMediumBlurEffect(mMediumThresholdFilteredImage),
    mSmallBlurEffect(mLargeThresholdFilteredImage),

    mSourceFramebuffer(thresholdFilteredImage->size()),
    mSmallFramebuffer(mSmallThresholdFilteredImage->size()),
    mMediumFramebuffer(mMediumThresholdFilteredImage->size()),
    mLargeFramebuffer(mLargeThresholdFilteredImage->size())
    {

    }

}
