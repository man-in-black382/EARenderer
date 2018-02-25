//
//  GLTexture2DSampler.cpp
//  EARenderer
//
//  Created by Pavel Muratov on 12/28/17.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "GLTexture2DSampler.hpp"
#include "GLTexture2D.hpp"

#include <glm/gtc/packing.hpp>

namespace EARenderer {
    
#pragma mark - Lifecycle
    
    GLTexture2DSampler::GLTexture2DSampler(const GLTexture2D* texture, int32_t mipLevel)
    :
    mTexture(texture),
    mPixelBuffer(texture->pixelBuffer(mipLevel)),
    mMipSize(texture->size().transformedBy(glm::vec2(std::pow(0.5, mipLevel))))
    { }
    
    GLTexture2DSampler::~GLTexture2DSampler() {
        delete mPixelBuffer;
    }
    
#pragma mark - Sampling
    
    Color GLTexture2DSampler::sample(int32_t x, int32_t y) const {
        int32_t offset = (y * mMipSize.width + x) * 4;
        return Color((float)mPixelBuffer[offset] / 255.0f,
                     (float)mPixelBuffer[offset + 1] / 255.0f,
                     (float)mPixelBuffer[offset + 2] / 255.0f,
                     (float)mPixelBuffer[offset + 3] / 255.0f);
    }

    Color GLTexture2DSampler::sample(const glm::vec2& normalizedCoords) const {
        int32_t x = normalizedCoords.x * (mMipSize.width - 1);
        int32_t y = normalizedCoords.y * (mMipSize.height - 1);
        int32_t offset = (y * mMipSize.width + x) * 4;

        return Color((float)mPixelBuffer[offset] / 255.0f,
                     (float)mPixelBuffer[offset + 1] / 255.0f,
                     (float)mPixelBuffer[offset + 2] / 255.0f,
                     (float)mPixelBuffer[offset + 3] / 255.0f);
    }
    
}
