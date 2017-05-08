//
//  GLFramebuffer.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 29.04.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "GLFramebuffer.hpp"
#include <OpenGL/gl3.h>
#include "Macros.h"

namespace EARenderer {
    
    GLFramebuffer::GLFramebuffer(const Size& size)
    :
    mSize(size)
    {
        ASSERT(size.width > 0, "Framebuffer width should be greater than 0");
        ASSERT(size.height > 0, "Framebuffer height should be greater than 0");
        
        GLuint name = 0;
        glGenFramebuffers(1, &name);
        mName = name;        
    }
    
    GLFramebuffer::~GLFramebuffer() {
        glDeleteFramebuffers(1, &mName);
    }
    
    void GLFramebuffer::bind() const {
        glBindFramebuffer(GL_FRAMEBUFFER, mName);
    }
    
    const Size& GLFramebuffer::size() const {
        return mSize;
    }
    
    bool GLFramebuffer::isComplete() {
        return glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
    }
    
    void GLFramebuffer::attachTexture(const GLDepthTexture2D& texture) {
        ASSERT(mSize == texture.size(), "Texture attachment must be of size that is expected by framebuffer (w: " << mSize.width << ", h: " << mSize.height << ")");
        
        bind();
        texture.bind();
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texture.getName(), 0);
        
        // Tell opengl that color attachments are not needed to complete the framebuffer
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
    }
    
}
