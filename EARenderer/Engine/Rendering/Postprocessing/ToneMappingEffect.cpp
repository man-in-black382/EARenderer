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

    ToneMappingEffect::ToneMappingEffect()
    :
    mHistogram(Size2D(64, 1)),
    mLuminance(Size2D(1))
    { }

#pragma mark - Tone mapping

    void ToneMappingEffect::buildHistogram(std::shared_ptr<const PostprocessTexturePool::PostprocessTexture> image,
                                           std::shared_ptr<PostprocessTexturePool> texturePool)
    {
        // Efficient Histogram Generation Using Scattering on GPUs
        // https://developer.amd.com/wordpress/media/2012/10/GPUHistogramGeneration_preprint.pdf

        mHistogramShader.bind();
        mHistogramShader.ensureSamplerValidity([&]() {
            mHistogramShader.setImage(*image);
            mHistogramShader.setHistogramWidth(mHistogram.size().width);
        });

        texturePool->redirectRenderingToTextures(GLViewport(mHistogram.size()), &mHistogram);

        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_ONE);
        Point::Draw(image->size().width * image->size().height);
        glDisable(GL_BLEND);
    }
    
    void ToneMappingEffect::toneMap(std::shared_ptr<const PostprocessTexturePool::PostprocessTexture> inputImage,
                                    std::shared_ptr<PostprocessTexturePool::PostprocessTexture> outputImage,
                                    std::shared_ptr<PostprocessTexturePool> texturePool)
    {
        buildHistogram(inputImage, texturePool);

        mToneMappingShader.bind();
        mToneMappingShader.ensureSamplerValidity([&]() {
            mToneMappingShader.setImage(*inputImage);
//            mToneMappingShader.setLuminance(*luminance);
        });

        texturePool->redirectRenderingToTexturesMip(0, outputImage);
        TriangleStripQuad::Draw();
    }

}
