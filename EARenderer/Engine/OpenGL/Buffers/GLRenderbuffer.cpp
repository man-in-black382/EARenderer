//
//  GLRenderbuffer.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 10.12.2017.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "GLRenderbuffer.hpp"

#include "Macros.h"

namespace EARenderer {
    
#pragma mark - Lifecycle
    
    GLRenderbuffer::GLRenderbuffer(const Size2D& size, GLenum internalFormat)
    :
    mSize(size)
    {
        ASSERT(size.width > 0, "Renderbuffer width should be greater than 0");
        ASSERT(size.height > 0, "Renderbuffer height should be greater than 0");
        
        glGenRenderbuffers(1, &mName);
        bind();
        glRenderbufferStorage(GL_RENDERBUFFER, internalFormat, size.width, size.height);
    }
    
    GLRenderbuffer::~GLRenderbuffer() {
        glDeleteRenderbuffers(1, &mName);
    }
    
#pragma mark - Binding
    
    void GLRenderbuffer::bind() const {
        glBindRenderbuffer(GL_RENDERBUFFER, mName);
    }
    
#pragma mark - Getters
    
    const Size2D& GLRenderbuffer::size() const {
        return mSize;
    }
    
}
