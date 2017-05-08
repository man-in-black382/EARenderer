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
    
    GLTexture2D::GLTexture2D(const std::string& fileName) {
        GLuint name = 0;
        glGenTextures(1, &name);
        mName = name;
        
        int32_t width = 0;
        int32_t height = 0;
        int32_t components = 0;
        stbi_uc *pixelData = stbi_load(fileName.c_str(), &width, &height, &components, STBI_rgb_alpha);
        
        ASSERT(pixelData, "Unable to read texture file: " << fileName);
        
        bind();
        
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixelData);
        
        stbi_image_free(pixelData);
    };
    
    GLTexture2D::~GLTexture2D() {
        glDeleteTextures(1, &mName);
    }
    
    void GLTexture2D::bind() const {
        glBindTexture(GL_TEXTURE_2D, mName);
    }
    
}
