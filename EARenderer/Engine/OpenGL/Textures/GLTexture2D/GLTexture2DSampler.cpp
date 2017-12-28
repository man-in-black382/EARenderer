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
    
    GLTexture2DSampler::GLTexture2DSampler(const GLTexture2D* texture)
    :
    mTexture(texture),
    mPixelBuffer(texture->pixelBuffer())
    { }
    
    GLTexture2DSampler::~GLTexture2DSampler() {
        delete mPixelBuffer;
    }
    
#pragma mark - Sampling
    
    Color GLTexture2DSampler::sample(int32_t x, int32_t y) const {
        int32_t offset = y * mTexture->size().width + x * 4;
        return Color((float)mPixelBuffer[offset] / 255.0f,
                     (float)mPixelBuffer[offset + 1] / 255.0f,
                     (float)mPixelBuffer[offset + 2] / 255.0f,
                     1.0);
    }
    
}
