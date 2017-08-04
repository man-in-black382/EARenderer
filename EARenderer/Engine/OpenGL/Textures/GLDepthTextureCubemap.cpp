//
//  GLDepthTextureCubemap.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 24.07.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "GLDepthTextureCubemap.hpp"
#include "Macros.h"

namespace EARenderer {
    
#pragma mark - Lifecycle
    
    GLDepthTextureCubemap::GLDepthTextureCubemap(const Size2D& size)
    :
    mSize(size)
    {
        ASSERT(size.width > 0, "Depth texture width should be greater than 0");
        ASSERT(size.height > 0, "Depth texture height should be greater than 0");
        
        GLuint name = -1;
        glGenTextures(1, &name);
        mName = name;
        
        bind();
        
        for(GLuint i = 0; i < 6; i++) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, size.width,
                         size.height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
        }
        
        applyParameters();
    }
    
    GLDepthTextureCubemap::~GLDepthTextureCubemap() {
        glDeleteTextures(1, &mName);
    }
    
    void GLDepthTextureCubemap::applyParameters() {
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    }
    
#pragma mark - Getters
    
    const Size2D& GLDepthTextureCubemap::size() const {
        return mSize;
    }
    
#pragma mark - Binding
    
    void GLDepthTextureCubemap::bind() const {
        glBindTexture(GL_TEXTURE_CUBE_MAP, mName);
    }
    
}
