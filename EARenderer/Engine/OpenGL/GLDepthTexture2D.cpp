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
    
    GLDepthTexture2D::GLDepthTexture2D(const Size& size)
    :
    mSize(size)
    {
        ASSERT(size.width > 0, "Depth texture width should be greater than 0");
        ASSERT(size.height > 0, "Depth texture height should be greater than 0");
        
        GLuint name = 0;
        glGenTextures(1, &name);
        mName = name;
        
        bind();
        
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, size.width, size.height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    }
    
    GLDepthTexture2D::~GLDepthTexture2D() {
        glDeleteTextures(1, &mName);
    }
    
    const Size& GLDepthTexture2D::size() const {
        return mSize;
    }
    
    void GLDepthTexture2D::bind() const {
        glBindTexture(GL_TEXTURE_2D, mName);
    }
    
}
