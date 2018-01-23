//
//  GLTexture2D.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 19.03.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "GLTexture2D.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "Macros.h"

namespace EARenderer {
    
#pragma mark - Lifecycle
    
    GLTexture2D::GLTexture2D(const Size2D& size)
    :
    GLTexture(size, GL_TEXTURE_2D, GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE)
    {
        ASSERT(size.width > 0, "Texture width should be greater than 0");
        ASSERT(size.height > 0, "Texture height should be greater than 0");

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, size.width, size.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    }
    
    GLTexture2D::GLTexture2D(const std::string& fileName)
    :
    GLTexture(GL_TEXTURE_2D, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT)
    {
        int32_t width = 0;
        int32_t height = 0;
        int32_t components = 0;
        stbi_uc *pixelData = stbi_load(fileName.c_str(), &width, &height, &components, STBI_rgb_alpha);
        
        ASSERT(pixelData, "Unable to read texture file: " << fileName);
        ASSERT(width > 0 && height > 0, "Unable to read texture file: " << fileName);
        
        mSize = Size2D(width, height);
        
        GLint texComponents;
        switch (components) {
            case 1: texComponents = GL_RED; break;
            case 2: texComponents = GL_RG; break;
            case 3: texComponents = GL_RGB; break;
            default: texComponents = GL_RGBA; break;
        }
        
        glTexImage2D(GL_TEXTURE_2D, 0, texComponents, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixelData);
        stbi_image_free(pixelData);
        
        generateMipmaps();
    };
    
#pragma mark - Sampling
    
    const GLubyte* GLTexture2D::pixelBuffer(int32_t mipLevel) const {
        bind();
        GLubyte *ptr = new GLubyte[mSize.width * mSize.height * 4];
        glGetTexImage(GL_TEXTURE_2D, mipLevel, GL_RGBA, GL_UNSIGNED_BYTE, ptr);
        return ptr;
    }
    
    void GLTexture2D::sampleTexels(SamplerClosure samplerClosure, int32_t mipLevel) const {
        bind();
        samplerClosure(GLTexture2DSampler(this, mipLevel));
    }
 
}