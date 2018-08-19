//
//  ToneMappingEffect.hpp
//  EARenderer
//
//  Created by Pavel Muratov on 6/27/18.
//  Copyright © 2018 MPO. All rights reserved.
//

#ifndef ToneMappingEffect_hpp
#define ToneMappingEffect_hpp

#include "GLTexture2D.hpp"
#include "GaussianBlurEffect.hpp"
#include "GLFramebuffer.hpp"
#include "GLSLLuminance.hpp"
#include "GLSLLuminanceRange.hpp"
#include "GLSLToneMapping.hpp"
#include "GLSLLuminanceHistogram.hpp"
#include "GLSLExposure.hpp"
#include "PostprocessTexturePool.hpp"

#include <memory>

namespace EARenderer {

    // Efficient Histogram Generation Using Scattering on GPUs
    // https://developer.amd.com/wordpress/media/2012/10/GPUHistogramGeneration_preprint.pdf

    class ToneMappingEffect {
    public:
        GLSLLuminance mLuminanceShader;
        GLSLLuminanceRange mLuminanceRangeShader;
        GLSLLuminanceHistogram mHistogramShader;
        GLSLExposure mExposureShader;
        GLSLToneMapping mToneMappingShader;
        GLFloatTexture2D<GLTexture::Float::RG16F> mLuminance;
        GLFloatTexture2D<GLTexture::Float::R32F> mHistogram;
        GLFloatTexture2D<GLTexture::Float::R16F> mExposure;

        void measureLuminance(std::shared_ptr<const PostprocessTexturePool::PostprocessTexture> image,
                              std::shared_ptr<PostprocessTexturePool> texturePool);

        void computeLuminanceRange(std::shared_ptr<PostprocessTexturePool> texturePool);

        void buildHistogram(std::shared_ptr<PostprocessTexturePool> texturePool);

        void computeExposure(std::shared_ptr<PostprocessTexturePool> texturePool);

    public:
        ToneMappingEffect(const Size2D& resolution);

        void toneMap(std::shared_ptr<const PostprocessTexturePool::PostprocessTexture> inputImage,
                     std::shared_ptr<PostprocessTexturePool::PostprocessTexture> outputImage,
                     std::shared_ptr<PostprocessTexturePool> texturePool);
    };

}

#endif /* ToneMappingEffect_hpp */
