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

    ToneMappingEffect::ToneMappingEffect(const Size2D& resolution)
    :
    mLuminance(resolution),
    mHistogram(Size2D(64, 1)),
    mExposure(Size2D(1))
    {
        // Luminance mip maps wiil be used to compute and store
        // minimum and maximum luminances
        mLuminance.generateMipMaps();
    }

#pragma mark - Private Interface

    void ToneMappingEffect::measureLuminance(std::shared_ptr<const PostprocessTexturePool::PostprocessTexture> image,
                                             std::shared_ptr<PostprocessTexturePool> texturePool)
    {
        mLuminanceShader.bind();
        mLuminanceShader.ensureSamplerValidity([&]() {
            mLuminanceShader.setImage(*image);
        });

        texturePool->redirectRenderingToTextures(GLViewport(mLuminance.size()), &mLuminance);
        TriangleStripQuad::Draw();
    }

    void ToneMappingEffect::computeLuminanceRange(std::shared_ptr<PostprocessTexturePool> texturePool) {
        mLuminanceRangeShader.bind();
        mLuminanceRangeShader.ensureSamplerValidity([&]() {
            mLuminanceRangeShader.setLuminance(mLuminance);
        });

        for (size_t mipLevel = 0; mipLevel < mLuminance.mipMapCount(); mipLevel++) {
            mLuminanceRangeShader.setMipLevel(mipLevel);
            texturePool->redirectRenderingToTexturesMip(mipLevel + 1, &mLuminance);
            TriangleStripQuad::Draw();
        }
    }

    void ToneMappingEffect::buildHistogram(std::shared_ptr<PostprocessTexturePool> texturePool) {
        mHistogramShader.bind();
        mHistogramShader.ensureSamplerValidity([&]() {
            mHistogramShader.setLuminance(mLuminance);
            mHistogramShader.setHistogramWidth(mHistogram.size().width);
        });

        texturePool->redirectRenderingToTextures(GLViewport(mHistogram.size()), &mHistogram);

        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_ONE);
        Point::Draw(mLuminance.size().width * mLuminance.size().height);
        glDisable(GL_BLEND);
    }

    void ToneMappingEffect::computeExposure(std::shared_ptr<PostprocessTexturePool> texturePool) {
        mExposureShader.bind();
        mExposureShader.ensureSamplerValidity([&]() {
            mExposureShader.setLuminanceHistogram(mHistogram);
        });

        texturePool->redirectRenderingToTextures(GLViewport(mExposure.size()), &mExposure);
        TriangleStripQuad::Draw();
    }

#pragma mark - Public Interface

    void ToneMappingEffect::toneMap(std::shared_ptr<const PostprocessTexturePool::PostprocessTexture> inputImage,
                                    std::shared_ptr<PostprocessTexturePool::PostprocessTexture> outputImage,
                                    std::shared_ptr<PostprocessTexturePool> texturePool)
    {
//        measureLuminance(inputImage, texturePool);
//        computeLuminanceRange(texturePool);
//        buildHistogram(texturePool);
//        computeExposure(texturePool);

        mToneMappingShader.bind();
        mToneMappingShader.ensureSamplerValidity([&]() {
            mToneMappingShader.setImage(*inputImage);
            mToneMappingShader.setExposure(mExposure);
        });

        texturePool->redirectRenderingToTexturesMip(0, outputImage);
        TriangleStripQuad::Draw();
    }

}
