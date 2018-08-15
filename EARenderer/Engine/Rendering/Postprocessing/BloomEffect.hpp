//
//  BloomFilter.hpp
//  EARenderer
//
//  Created by Pavel Muratov on 6/26/18.
//  Copyright © 2018 MPO. All rights reserved.
//

#ifndef BloomFilter_hpp
#define BloomFilter_hpp

#include "GLTexture2D.hpp"
#include "GaussianBlurEffect.hpp"
#include "GLFramebuffer.hpp"
#include "GLSLBloom.hpp"
#include "BloomSettings.hpp"

#include <memory>

namespace EARenderer {

    class BloomEffect {
    private:
        GaussianBlurEffect mSmallBlurEffect;
        GaussianBlurEffect mMediumBlurEffect;
        GaussianBlurEffect mLargeBlurEffect;

        GLSLBloom mBloomShader;

    public:
        void bloom(std::shared_ptr<const PostprocessTexturePool::PostprocessTexture> baseImage,
                   std::shared_ptr<PostprocessTexturePool::PostprocessTexture> thresholdFilteredImage,
                   std::shared_ptr<PostprocessTexturePool::PostprocessTexture> outputImage,
                   std::shared_ptr<GLFloatTexture2D<GLTexture::Float::R16F>> outputLuminance,
                   std::shared_ptr<PostprocessTexturePool> texturePool,
                   const BloomSettings& settings);
    };

}

#endif /* BloomFilter_hpp */
