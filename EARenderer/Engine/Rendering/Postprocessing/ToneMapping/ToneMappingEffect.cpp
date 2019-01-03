//
//  ToneMappingEffect.cpp
//  EARenderer
//
//  Created by Pavel Muratov on 6/27/18.
//  Copyright © 2018 MPO. All rights reserved.
//

#include "ToneMappingEffect.hpp"
#include "Drawable.hpp"

namespace EARenderer {

#pragma mark - Lifecycle

    ToneMappingEffect::ToneMappingEffect(std::shared_ptr<GLFramebuffer> sharedFramebuffer, std::shared_ptr<PostprocessTexturePool> sharedTexturePool)
            : PostprocessEffect(sharedFramebuffer, sharedTexturePool),
              mLuminance(sharedFramebuffer->size()),
              mHistogram(Size2D(64, 1)),
              mExposure(Size2D(1)) {
        // Luminance mip maps will be used to compute and store
        // minimum and maximum luminance
        mLuminance.generateMipMaps();
    }

#pragma mark - Private Interface

    void ToneMappingEffect::measureLuminance(std::shared_ptr<const typename PostprocessTexturePool::PostprocessTexture> image) {
        mLuminanceShader.bind();
        mLuminanceShader.ensureSamplerValidity([&]() {
            mLuminanceShader.setImage(*image);
        });

//        mFramebuffer->redirectRenderingToTextures(GLViewport(mLuminance.size()), &mLuminance);
        Drawable::TriangleStripQuad::Draw();
    }

    void ToneMappingEffect::computeLuminanceRange() {
        mLuminanceRangeShader.bind();
        mLuminanceRangeShader.ensureSamplerValidity([&]() {
            mLuminanceRangeShader.setLuminance(mLuminance);
        });

        for (size_t mipLevel = 0; mipLevel < mLuminance.mipMapCount(); mipLevel++) {
            mLuminanceRangeShader.setMipLevel(mipLevel);
//            mFramebuffer->redirectRenderingToTexturesMip(mipLevel + 1, &mLuminance);
            Drawable::TriangleStripQuad::Draw();
        }
    }

    void ToneMappingEffect::buildHistogram() {
        mHistogramShader.bind();
        mHistogramShader.ensureSamplerValidity([&]() {
            mHistogramShader.setLuminance(mLuminance);
            mHistogramShader.setHistogramWidth(mHistogram.size().width);
        });

//        mFramebuffer->redirectRenderingToTextures(GLViewport(mHistogram.size()), &mHistogram);

        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_ONE);
        Drawable::Point::Draw(mLuminance.size().width * mLuminance.size().height);
        glDisable(GL_BLEND);
    }

    void ToneMappingEffect::computeExposure() {
        mExposureShader.bind();
        mExposureShader.ensureSamplerValidity([&]() {
            mExposureShader.setLuminanceHistogram(mHistogram);
        });

//        mFramebuffer->redirectRenderingToTextures(GLViewport(mExposure.size()), &mExposure);
        Drawable::TriangleStripQuad::Draw();
    }

#pragma mark - Public Interface

    void ToneMappingEffect::toneMap(std::shared_ptr<const PostprocessTexturePool::PostprocessTexture> inputImage,
            std::shared_ptr<PostprocessTexturePool::PostprocessTexture> outputImage) {
        //        measureLuminance(inputImage, texturePool);
        //        computeLuminanceRange(texturePool);
        //        buildHistogram(texturePool);
        //        computeExposure(texturePool);

        mToneMappingShader.bind();
        mToneMappingShader.ensureSamplerValidity([&]() {
            mToneMappingShader.setImage(*inputImage);
//            mToneMappingShader.setExposure(mExposure);
        });

        mFramebuffer->redirectRenderingToTextures(GLFramebuffer::UnderlyingBuffer::None, outputImage);
        Drawable::TriangleStripQuad::Draw();
    }

}
