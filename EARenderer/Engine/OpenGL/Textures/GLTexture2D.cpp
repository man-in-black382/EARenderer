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
    
    GLTexture2D::GLTexture2D(const Size2D& size) {
        ASSERT(size.width > 0, "Texture width should be greater than 0");
        ASSERT(size.height > 0, "Texture height should be greater than 0");
        
        GLuint name = -1;
        glGenTextures(1, &name);
        mName = name;
        mSize = size;
        
        bind();
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, size.width, size.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        applyParameters();
    }
    
    GLTexture2D::GLTexture2D(const std::string& fileName) {
        GLuint name = -1;
        glGenTextures(1, &name);
        mName = name;
        
        int32_t width = 0;
        int32_t height = 0;
        int32_t components = 0;
        stbi_uc *pixelData = stbi_load(fileName.c_str(), &width, &height, &components, STBI_rgb_alpha);
        
        ASSERT(pixelData, "Unable to read texture file: " << fileName);
        
        mSize = Size2D(width, height);
        
        bind();
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixelData);
        applyParameters();
        stbi_image_free(pixelData);
    };
    
    GLTexture2D::~GLTexture2D() {
        glDeleteTextures(1, &mName);
    }
    
#pragma mark - Private
    
    void GLTexture2D::applyParameters() {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    }
    
#pragma mark - Public
    
    void GLTexture2D::bind() const {
        glBindTexture(GL_TEXTURE_2D, mName);
    }
    
    const Size2D& GLTexture2D::size() const {
        return mSize;
    }
    
}
