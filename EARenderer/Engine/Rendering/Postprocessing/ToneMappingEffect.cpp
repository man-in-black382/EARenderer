//
//  ToneMappingEffect.cpp
//  EARenderer
//
//  Created by Pavel Muratov on 6/27/18.
//  Copyright © 2018 MPO. All rights reserved.
//

#include "ToneMappingEffect.hpp"

namespace EARenderer {

#pragma mark - Lifecycle

    ToneMappingEffect::ToneMappingEffect(std::shared_ptr<const GLHDRTexture2D> inputImage)
    :
    mInputImage(inputImage),
    mOutputImage(std::make_shared<GLHDRTexture2D>(inputImage->size())),
    mFramebuffer(inputImage->size())
    {
        mFramebuffer.attachTexture(*mOutputImage);
    }

#pragma mark - Getters

    std::shared_ptr<GLHDRTexture2D> ToneMappingEffect::outputImage() const {
        return mOutputImage;
    }

#pragma mark - Tone mapping

    std::shared_ptr<GLHDRTexture2D> ToneMappingEffect::toneMap() {
        mFramebuffer.bind();
        mFramebuffer.viewport().apply();

        mToneMappingShader.bind();
        mToneMappingShader.ensureSamplerValidity([&]() {
            mToneMappingShader.setImage(*mInputImage);
        });

        glClear(GL_COLOR_BUFFER_BIT);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        return mOutputImage;
    }

}
