//
//  GLLDRTexture2DSampler.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 18.03.2018.
//  Copyright © 2018 MPO. All rights reserved.
//

#include "GLLDRTexture2DSampler.hpp"
#include "GLLDRTexture2D.hpp"

namespace EARenderer {

#pragma mark - Lifecycle

    GLLDRTexture2DSampler::GLLDRTexture2DSampler(const GLLDRTexture2D& texture, uint8_t mipLevel)
    :
    GLTexture2DSampler(texture, mipLevel),
    mPixelBuffer(new GLubyte[mMipSize.width * mMipSize.height * 4])
    {
        glGetTexImage(GL_TEXTURE_2D, mipLevel, GL_RGBA, GL_UNSIGNED_BYTE, mPixelBuffer.get());
    }

#pragma mark - Sampling

    Color GLLDRTexture2DSampler::sample(int32_t x, int32_t y) const {
        size_t offset = bufferOffset(x, y);
        GLubyte *buffer = mPixelBuffer.get();
        return Color((float)buffer[offset] / 255.0f,
                     (float)buffer[offset + 1] / 255.0f,
                     (float)buffer[offset + 2] / 255.0f,
                     (float)buffer[offset + 3] / 255.0f);
    }

    Color GLLDRTexture2DSampler::sample(const glm::vec2& normalizedCoords) const {
        size_t offset = bufferOffset(normalizedCoords);
        GLubyte *buffer = mPixelBuffer.get();
        return Color((float)buffer[offset] / 255.0f,
                     (float)buffer[offset + 1] / 255.0f,
                     (float)buffer[offset + 2] / 255.0f,
                     (float)buffer[offset + 3] / 255.0f);
    }

}
