//
//  BloomFilter.cpp
//  EARenderer
//
//  Created by Pavel Muratov on 6/26/18.
//  Copyright © 2018 MPO. All rights reserved.
//

#include "BloomEffect.hpp"

namespace EARenderer {

#pragma mark - Lifecycle

    BloomEffect::BloomEffect(std::shared_ptr<const GLHDRTexture2D> baseImage,
                             std::shared_ptr<const GLHDRTexture2D> thresholdFilteredImage)
    :
    mBaseImage(baseImage),
    mThresholdFilteredImage(thresholdFilteredImage),
    mOutputImage(std::make_shared<GLHDRTexture2D>(baseImage->size())),

    mLargeThresholdFilteredImage(std::make_shared<GLHDRTexture2D>(thresholdFilteredImage->size())),
    mMediumThresholdFilteredImage(std::make_shared<GLHDRTexture2D>(thresholdFilteredImage->size().transformedBy(glm::vec2(0.5)))),
    mSmallThresholdFilteredImage(std::make_shared<GLHDRTexture2D>(thresholdFilteredImage->size().transformedBy(glm::vec2(0.25)))),

    mLargeBlurEffect(mSmallThresholdFilteredImage),
    mMediumBlurEffect(mMediumThresholdFilteredImage),
    mSmallBlurEffect(mLargeThresholdFilteredImage),

    mFramebuffer(thresholdFilteredImage->size())
//    mSmallFramebuffer(mSmallThresholdFilteredImage->size()),
//    mMediumFramebuffer(mMediumThresholdFilteredImage->size()),
//    mLargeFramebuffer(mLargeThresholdFilteredImage->size())
    {
        mFramebuffer.attachTexture(*mThresholdFilteredImage, GLFramebuffer::ColorAttachment::Attachment0);
        mFramebuffer.attachTexture(*mLargeThresholdFilteredImage, GLFramebuffer::ColorAttachment::Attachment1);
        mFramebuffer.attachTexture(*mMediumThresholdFilteredImage, GLFramebuffer::ColorAttachment::Attachment2);
        mFramebuffer.attachTexture(*mSmallThresholdFilteredImage, GLFramebuffer::ColorAttachment::Attachment3);
    }

#pragma mark - Filter

    std::shared_ptr<GLHDRTexture2D> BloomEffect::bloom() {
        mFramebuffer.blit(GLFramebuffer::ColorAttachment::Attachment0,
                          Rect2D(mThresholdFilteredImage->size()),
                          GLFramebuffer::ColorAttachment::Attachment1,
                          Rect2D(mLargeThresholdFilteredImage->size()));

        mFramebuffer.blit(GLFramebuffer::ColorAttachment::Attachment0,
                          Rect2D(mThresholdFilteredImage->size()),
                          GLFramebuffer::ColorAttachment::Attachment2,
                          Rect2D(mMediumThresholdFilteredImage->size()));

        mFramebuffer.blit(GLFramebuffer::ColorAttachment::Attachment0,
                          Rect2D(mThresholdFilteredImage->size()),
                          GLFramebuffer::ColorAttachment::Attachment3,
                          Rect2D(mSmallThresholdFilteredImage->size()));

        return mOutputImage;
    }

}
