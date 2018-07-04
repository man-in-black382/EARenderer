//
//  BloomFilter.hpp
//  EARenderer
//
//  Created by Pavel Muratov on 6/26/18.
//  Copyright © 2018 MPO. All rights reserved.
//

#ifndef BloomFilter_hpp
#define BloomFilter_hpp

#include "GLHDRTexture2D.hpp"
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
        void bloom(std::shared_ptr<const GLHDRTexture2D> baseImage,
                   std::shared_ptr<GLHDRTexture2D> thresholdFilteredImage,
                   std::shared_ptr<GLHDRTexture2D> outputImage,
                   std::shared_ptr<PostprocessTexturePool> texturePool,
                   const BloomSettings& settings);
    };

}

#endif /* BloomFilter_hpp */
