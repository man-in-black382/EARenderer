//
//  ToneMappingEffect.cpp
//  EARenderer
//
//  Created by Pavel Muratov on 6/27/18.
//  Copyright © 2018 MPO. All rights reserved.
//

#include "Drawable.hpp"

namespace EARenderer {
    
#pragma mark - Lifecycle
    
    template <GLTexture::Float TextureFormat>
    ToneMappingEffect<TextureFormat>::ToneMappingEffect(std::shared_ptr<GLFramebuffer> sharedFramebuffer,
                                                        std::shared_ptr<PostprocessTexturePool<TextureFormat>> sharedTexturePool)
    :
    PostprocessEffect<TextureFormat>(sharedFramebuffer, sharedTexturePool),
    mLuminance(sharedFramebuffer->size()),
    mHistogram(Size2D(64, 1)),
    mExposure(Size2D(1))
    {
        // Luminance mip maps wiil be used to compute and store
        // minimum and maximum luminances
        mLuminance.generateMipMaps();
    }
    
#pragma mark - Private Interface
    
    template <GLTexture::Float TextureFormat>
    void ToneMappingEffect<TextureFormat>::measureLuminance(std::shared_ptr<const typename PostprocessTexturePool<TextureFormat>::PostprocessTexture> image)
    {
        mLuminanceShader.bind();
        mLuminanceShader.ensureSamplerValidity([&]() {
            mLuminanceShader.setImage(*image);
        });
        
        this->mFramebuffer->redirectRenderingToTextures(GLViewport(mLuminance.size()), &mLuminance);
        Drawable::TriangleStripQuad::Draw();
    }
    
    template <GLTexture::Float TextureFormat>
    void ToneMappingEffect<TextureFormat>::computeLuminanceRange() {
        mLuminanceRangeShader.bind();
        mLuminanceRangeShader.ensureSamplerValidity([&]() {
            mLuminanceRangeShader.setLuminance(mLuminance);
        });
        
        for (size_t mipLevel = 0; mipLevel < mLuminance.mipMapCount(); mipLevel++) {
            mLuminanceRangeShader.setMipLevel(mipLevel);
            this->mFramebuffer->redirectRenderingToTexturesMip(mipLevel + 1, &mLuminance);
            Drawable::TriangleStripQuad::Draw();
        }
    }
    
    template <GLTexture::Float TextureFormat>
    void ToneMappingEffect<TextureFormat>::buildHistogram() {
        mHistogramShader.bind();
        mHistogramShader.ensureSamplerValidity([&]() {
            mHistogramShader.setLuminance(mLuminance);
            mHistogramShader.setHistogramWidth(mHistogram.size().width);
        });
        
        this->mFramebuffer->redirectRenderingToTextures(GLViewport(mHistogram.size()), &mHistogram);
        
        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_ONE);
        Drawable::Point::Draw(mLuminance.size().width * mLuminance.size().height);
        glDisable(GL_BLEND);
    }
    
    template <GLTexture::Float TextureFormat>
    void ToneMappingEffect<TextureFormat>::computeExposure() {
        mExposureShader.bind();
        mExposureShader.ensureSamplerValidity([&]() {
            mExposureShader.setLuminanceHistogram(mHistogram);
        });
        
        this->mFramebuffer->redirectRenderingToTextures(GLViewport(mExposure.size()), &mExposure);
        Drawable::TriangleStripQuad::Draw();
    }
    
#pragma mark - Public Interface
    
    template <GLTexture::Float TextureFormat>
    void ToneMappingEffect<TextureFormat>::toneMap(std::shared_ptr<const typename PostprocessTexturePool<TextureFormat>::PostprocessTexture> inputImage,
                                                   std::shared_ptr<typename PostprocessTexturePool<TextureFormat>::PostprocessTexture> outputImage)
    {
        //        measureLuminance(inputImage, texturePool);
        //        computeLuminanceRange(texturePool);
        //        buildHistogram(texturePool);
        //        computeExposure(texturePool);
        
        mToneMappingShader.bind();
        mToneMappingShader.ensureSamplerValidity([&]() {
            mToneMappingShader.setImage(*inputImage);
//            mToneMappingShader.setExposure(mExposure);
        });
        
        this->mFramebuffer->redirectRenderingToTextures(GLFramebuffer::UnderlyingBuffer::None, outputImage);
        Drawable::TriangleStripQuad::Draw();
    }
    
}
