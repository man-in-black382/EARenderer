//
//  GLHDRCubemapSampler.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 18.11.2017.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "GLHDRCubemapSampler.hpp"
#include "GLHDRTextureCubemap.hpp"

#include <glm/gtc/packing.hpp>

namespace EARenderer {
    
#pragma mark - Lifecycle
    
    GLHDRCubemapSampler::GLHDRCubemapSampler(const GLHDRTextureCubemap& texture, uint8_t mipLevel)
    :
    GLCubemapSampler(texture.size(), mipLevel),
    mPositiveXPixelBuffer(new GLhalf[mMipSize.width * mMipSize.height * 4]),
    mNegativeXPixelBuffer(new GLhalf[mMipSize.width * mMipSize.height * 4]),
    mPositiveYPixelBuffer(new GLhalf[mMipSize.width * mMipSize.height * 4]),
    mNegativeYPixelBuffer(new GLhalf[mMipSize.width * mMipSize.height * 4]),
    mPositiveZPixelBuffer(new GLhalf[mMipSize.width * mMipSize.height * 4]),
    mNegativeZPixelBuffer(new GLhalf[mMipSize.width * mMipSize.height * 4])
    {
        texture.bind();

        // Despite the fact that texture is being 3-component we treat it as 4-component as stated in the OpenGL docs:
        //
        // Three-component textures are treated as RGBA buffers with red set to component zero,
        // green set to component one, blue set to component two, and alpha set to 1

        glGetTexImage(0, mipLevel, GL_RGBA, GL_HALF_FLOAT, mPositiveXPixelBuffer.get());
        glGetTexImage(1, mipLevel, GL_RGBA, GL_HALF_FLOAT, mNegativeXPixelBuffer.get());
        glGetTexImage(2, mipLevel, GL_RGBA, GL_HALF_FLOAT, mPositiveYPixelBuffer.get());
        glGetTexImage(3, mipLevel, GL_RGBA, GL_HALF_FLOAT, mNegativeYPixelBuffer.get());
        glGetTexImage(4, mipLevel, GL_RGBA, GL_HALF_FLOAT, mPositiveZPixelBuffer.get());
        glGetTexImage(5, mipLevel, GL_RGBA, GL_HALF_FLOAT, mNegativeZPixelBuffer.get());
    }
    
#pragma mark - Sampling
    
    Color GLHDRCubemapSampler::sample(const glm::vec3& direction) const {
        GLCubemapFace face;
        glm::vec2 texCoords;
        computeTexCoords(direction, face, texCoords);
        return sample(face, texCoords.x * mMipSize.width, texCoords.y * mMipSize.height);
    }
    
    Color GLHDRCubemapSampler::sample(GLCubemapFace face, int32_t x, int32_t y) const {
        const GLhalf *pixelBuffer = nullptr;
        switch (face) {
            case GLCubemapFace::PositiveX: pixelBuffer = mPositiveXPixelBuffer.get(); break;
            case GLCubemapFace::NegativeX: pixelBuffer = mNegativeXPixelBuffer.get(); break;
            case GLCubemapFace::PositiveY: pixelBuffer = mPositiveYPixelBuffer.get(); break;
            case GLCubemapFace::NegativeY: pixelBuffer = mNegativeYPixelBuffer.get(); break;
            case GLCubemapFace::PositiveZ: pixelBuffer = mPositiveZPixelBuffer.get(); break;
            case GLCubemapFace::NegativeZ: pixelBuffer = mNegativeZPixelBuffer.get(); break;
        }
        
        int32_t offset = (y * mMipSize.width + x) * 4;
        return Color(glm::unpackHalf1x16(pixelBuffer[offset]),
                     glm::unpackHalf1x16(pixelBuffer[offset + 1]),
                     glm::unpackHalf1x16(pixelBuffer[offset + 2]),
                     glm::unpackHalf1x16(pixelBuffer[offset + 3]));
        
//        switch (face) {
//            case GLCubemapFace::PositiveX: return Color(1.0, 0.0, 0.0, 1.0);
//            case GLCubemapFace::NegativeX: return Color(1.0, 0.0, 0.0, 1.0);
//            case GLCubemapFace::PositiveY: return Color(0.0, 1.0, 0.0, 1.0);
//            case GLCubemapFace::NegativeY: return Color(0.0, 1.0, 0.0, 1.0);
//            case GLCubemapFace::PositiveZ: return Color(0.0, 0.0, 1.0, 1.0);
//            case GLCubemapFace::NegativeZ: return Color(0.0, 0.0, 1.0, 1.0);
//        }
    }
    
}
