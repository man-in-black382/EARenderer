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

#include <memory>

namespace EARenderer {

    class BloomEffect {
    private:
        std::shared_ptr<const GLHDRTexture2D> mBaseImage;
        std::shared_ptr<const GLHDRTexture2D> mSmallThresholdFilteredImage;
        std::shared_ptr<const GLHDRTexture2D> mMediumThresholdFilteredImage;
        std::shared_ptr<const GLHDRTexture2D> mLargeThresholdFilteredImage;
        std::shared_ptr<GLHDRTexture2D> mOutputImage;

        GaussianBlurEffect mSmallBlurEffect;
        GaussianBlurEffect mMediumBlurEffect;
        GaussianBlurEffect mLargeBlurEffect;

        GLFramebuffer mSourceFramebuffer;
        GLFramebuffer mSmallFramebuffer;
        GLFramebuffer mMediumFramebuffer;
        GLFramebuffer mLargeFramebuffer;

    public:
        BloomEffect(std::shared_ptr<const GLHDRTexture2D> baseImage,
                    std::shared_ptr<const GLHDRTexture2D> thresholdFilteredImage);
    };

}

#endif /* BloomFilter_hpp */
