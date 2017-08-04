//
//  GLDepthTexture2D.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 30.04.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "GLDepthTexture2D.hpp"
#include "Macros.h"

namespace EARenderer {
    
    GLDepthTexture2D::GLDepthTexture2D(const Size2D& size)
    :
    mSize(size)
    {
        ASSERT(size.width > 0, "Depth texture width should be greater than 0");
        ASSERT(size.height > 0, "Depth texture height should be greater than 0");
        
        GLuint name = -1;
        glGenTextures(1, &name);
        mName = name;
        
        bind();
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, size.width, size.height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
        applyParamaters();
    }
    
    GLDepthTexture2D::~GLDepthTexture2D() {
        glDeleteTextures(1, &mName);
    }
    
    void GLDepthTexture2D::applyParamaters() {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        // Prevent oversampling by supplying 1.0 depth values when texture is sampled beyond [0.0; 1.0] range
        GLfloat borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    }
    
#pragma mark - Getters
    
    const Size2D& GLDepthTexture2D::size() const {
        return mSize;
    }
    
#pragma mark - Binding
    
    void GLDepthTexture2D::bind() const {
        glBindTexture(GL_TEXTURE_2D, mName);
    }
    
}
