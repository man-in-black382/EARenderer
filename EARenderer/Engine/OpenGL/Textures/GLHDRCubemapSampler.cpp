//
//  GLHDRCubemapSampler.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 18.11.2017.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "GLHDRCubemapSampler.hpp"

#include <glm/gtc/packing.hpp>

namespace EARenderer {
    
#pragma mark - Lifecycle
    
    GLHDRCubemapSampler::GLHDRCubemapSampler(const GLHDRTextureCubemap* cubemap)
    :
    mCubemap(cubemap),
    mPositiveXPixelBuffer(cubemap->pixelBuffer(GLCubemapFace::PositiveX)),
    mNegativeXPixelBuffer(cubemap->pixelBuffer(GLCubemapFace::NegativeX)),
    mPositiveYPixelBuffer(cubemap->pixelBuffer(GLCubemapFace::PositiveY)),
    mNegativeYPixelBuffer(cubemap->pixelBuffer(GLCubemapFace::NegativeY)),
    mPositiveZPixelBuffer(cubemap->pixelBuffer(GLCubemapFace::PositiveZ)),
    mNegativeZPixelBuffer(cubemap->pixelBuffer(GLCubemapFace::NegativeZ))
    { }
    
    GLHDRCubemapSampler::~GLHDRCubemapSampler() {
        delete mPositiveXPixelBuffer;
        delete mNegativeXPixelBuffer;
        delete mPositiveYPixelBuffer;
        delete mNegativeYPixelBuffer;
        delete mPositiveZPixelBuffer;
        delete mNegativeZPixelBuffer;
    }
    
#pragma mark - Sampling
    
    Color GLHDRCubemapSampler::sample(const glm::vec3& direction) const {
        GLCubemapFace face;
        glm::vec2 texCoords;
        computeTexCoords(direction, face, texCoords);
        return sample(face, texCoords.x * mCubemap->size().width, texCoords.y * mCubemap->size().height);
    }
    
    Color GLHDRCubemapSampler::sample(GLCubemapFace face, int32_t x, int32_t y) const {
        const GLhalf *pixelBuffer = nullptr;
        switch (face) {
            case GLCubemapFace::PositiveX: pixelBuffer = mPositiveXPixelBuffer; break;
            case GLCubemapFace::NegativeX: pixelBuffer = mNegativeXPixelBuffer; break;
            case GLCubemapFace::PositiveY: pixelBuffer = mPositiveYPixelBuffer; break;
            case GLCubemapFace::NegativeY: pixelBuffer = mNegativeYPixelBuffer; break;
            case GLCubemapFace::PositiveZ: pixelBuffer = mPositiveZPixelBuffer; break;
            case GLCubemapFace::NegativeZ: pixelBuffer = mNegativeZPixelBuffer; break;
        }
        
        int32_t offset = y * mCubemap->size().width + x * 4;
        return Color(glm::unpackHalf1x16(pixelBuffer[offset]),
                     glm::unpackHalf1x16(pixelBuffer[offset + 1]),
                     glm::unpackHalf1x16(pixelBuffer[offset + 2]),
                     1.0);
    }
    
}
