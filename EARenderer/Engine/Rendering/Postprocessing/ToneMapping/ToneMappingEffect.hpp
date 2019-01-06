//
//  ToneMappingEffect.hpp
//  EARenderer
//
//  Created by Pavel Muratov on 6/27/18.
//  Copyright © 2018 MPO. All rights reserved.
//

#ifndef ToneMappingEffect_hpp
#define ToneMappingEffect_hpp

#include "PostprocessEffect.hpp"
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

    class ToneMappingEffect : public PostprocessEffect {
    private:
        GLSLLuminance mLuminanceShader;
        GLSLLuminanceRange mLuminanceRangeShader;
        GLSLLuminanceHistogram mHistogramShader;
        GLSLExposure mExposureShader;
        GLSLToneMapping mToneMappingShader;
        GLFloatTexture2D<GLTexture::Float::RG16F> mLuminance;
        GLFloatTexture2D<GLTexture::Float::R32F> mHistogram;
        GLFloatTexture2D<GLTexture::Float::R16F> mExposure;

        void measureLuminance(const PostprocessTexturePool::PostprocessTexture &image);

        void computeLuminanceRange();

        void buildHistogram();

        void computeExposure();

    public:
        ToneMappingEffect(std::shared_ptr<GLFramebuffer> sharedFramebuffer, std::shared_ptr<PostprocessTexturePool> sharedTexturePool);

        void toneMap(const PostprocessTexturePool::PostprocessTexture &inputImage, PostprocessTexturePool::PostprocessTexture &outputImage);
    };

}

#endif /* ToneMappingEffect_hpp */
