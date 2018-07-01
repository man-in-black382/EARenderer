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
    mOutputFramebuffer(mOutputImage->size()),

    mLargeThresholdFilteredImage(std::make_shared<GLHDRTexture2D>(thresholdFilteredImage->size())),
    mMediumThresholdFilteredImage(std::make_shared<GLHDRTexture2D>(thresholdFilteredImage->size().transformedBy(glm::vec2(0.5)))),
    mSmallThresholdFilteredImage(std::make_shared<GLHDRTexture2D>(thresholdFilteredImage->size().transformedBy(glm::vec2(0.25)))),

    mLargeBlurEffect(mSmallThresholdFilteredImage),
    mMediumBlurEffect(mMediumThresholdFilteredImage),
    mSmallBlurEffect(mLargeThresholdFilteredImage),

    mFramebuffer(thresholdFilteredImage->size())
    {
        mFramebuffer.attachTexture(*mThresholdFilteredImage, GLFramebuffer::ColorAttachment::Attachment0);
        mFramebuffer.attachTexture(*mLargeThresholdFilteredImage, GLFramebuffer::ColorAttachment::Attachment1);
        mFramebuffer.attachTexture(*mMediumThresholdFilteredImage, GLFramebuffer::ColorAttachment::Attachment2);
        mFramebuffer.attachTexture(*mSmallThresholdFilteredImage, GLFramebuffer::ColorAttachment::Attachment3);

        mOutputFramebuffer.attachTexture(*mOutputImage);
    }

#pragma mark -

    std::shared_ptr<GLHDRTexture2D> BloomEffect::outputImage() const {
        return mOutputImage;
    }

    std::shared_ptr<GLHDRTexture2D> BloomEffect::bloom(const BloomSettings& settings) {
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

        mSmallBlurEffect.blur(settings.smallBlurSettings);
        mMediumBlurEffect.blur(settings.mediumBlurSettings);
        mLargeBlurEffect.blur(settings.largeBlurSettings);

        float totalWeight = settings.smallBlurWeight + settings.mediumBlurWeight + settings.largeBlurWeight;
        float smallBlurWeightNorm = settings.smallBlurWeight / totalWeight * settings.bloomStrength;
        float mediumBlurWeightNorm = settings.mediumBlurWeight / totalWeight * settings.bloomStrength;
        float largeBlurWeightNorm = settings.largeBlurWeight / totalWeight * settings.bloomStrength;

        mBloomShader.bind();
        mBloomShader.ensureSamplerValidity([&]() {
            mBloomShader.setTextures(*mBaseImage,
                                     *mSmallBlurEffect.outputImage(),
                                     *mMediumBlurEffect.outputImage(),
                                     *mLargeBlurEffect.outputImage());

            mBloomShader.setTextureWeights(smallBlurWeightNorm, mediumBlurWeightNorm, largeBlurWeightNorm);
        });

        mOutputFramebuffer.bind();
        mOutputFramebuffer.viewport().apply();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        return mOutputImage;
    }

}
