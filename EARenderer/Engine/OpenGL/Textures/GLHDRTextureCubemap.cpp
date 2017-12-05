//
//  GLHDRTextureCubemap.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 09.10.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "GLHDRTextureCubemap.hpp"
#include "GLHDRCubemapSampler.hpp"
#include "Macros.h"
#include "stb_image.h"

namespace EARenderer {
    
#pragma mark - Lifecycle
    
    GLHDRTextureCubemap::GLHDRTextureCubemap(const Size2D& size)
    :
    GLTexture(size,
              GL_TEXTURE_CUBE_MAP,
              GL_LINEAR,
              GL_LINEAR,
              GL_CLAMP_TO_EDGE,
              GL_CLAMP_TO_EDGE)
    {
        ASSERT(size.width > 0, "Cubemap texture width should be greater than 0");
        ASSERT(size.height > 0, "Cubemap texture height should be greater than 0");
        
        for(GLuint i = 0; i < 6; i++) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, size.width, size.height, 0, GL_RGB, GL_FLOAT, nullptr);
        }
    }
    
#pragma mark - Public interface
    
    const GLhalf* GLHDRTextureCubemap::pixelBuffer(GLCubemapFace face) const {
        bind();
        GLhalf *ptr = new GLhalf[mSize.width * mSize.height * 3];
        using type = std::underlying_type<GLCubemapFace>::type;
        type rawFace = static_cast<type>(face);
        glGetTexImage(rawFace, 0, GL_RGB, GL_HALF_FLOAT, ptr);
        return ptr;
    }
    
    void GLHDRTextureCubemap::sampleTexels(SamplerClosure samplerClosure) const {
        bind();
        samplerClosure(GLHDRCubemapSampler(this));
    }
    
}


