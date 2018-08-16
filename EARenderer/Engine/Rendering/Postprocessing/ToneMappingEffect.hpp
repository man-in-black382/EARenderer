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
#include "GLSLToneMapping.hpp"
#include "GLSLLuminanceHistogram.hpp"
#include "PostprocessTexturePool.hpp"

#include <memory>

namespace EARenderer {

    class ToneMappingEffect {
    public:
        GLSLLuminanceHistogram mHistogramShader;
        GLSLToneMapping mToneMappingShader;
        GLFloatTexture2D<GLTexture::Float::R32F> mHistogram;
        GLFloatTexture2D<GLTexture::Float::R16F> mLuminance;

        void buildHistogram(std::shared_ptr<const PostprocessTexturePool::PostprocessTexture> image,
                            std::shared_ptr<PostprocessTexturePool> texturePool);

    public:
        ToneMappingEffect();

        void toneMap(std::shared_ptr<const PostprocessTexturePool::PostprocessTexture> inputImage,
                     std::shared_ptr<PostprocessTexturePool::PostprocessTexture> outputImage,
                     std::shared_ptr<PostprocessTexturePool> texturePool);
    };

}

#endif /* ToneMappingEffect_hpp */
