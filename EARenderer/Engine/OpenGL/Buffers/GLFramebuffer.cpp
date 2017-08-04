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
    
    GLFramebuffer::GLFramebuffer(const Size2D& size)
    :
    mSize(size)
    {
        ASSERT(size.width > 0, "Framebuffer width should be greater than 0");
        ASSERT(size.height > 0, "Framebuffer height should be greater than 0");
        
        GLuint name = -1;
        glGenFramebuffers(1, &name);
        mName = name;        
    }
    
    GLFramebuffer::~GLFramebuffer() {
        glDeleteFramebuffers(1, &mName);
    }
    
    void GLFramebuffer::bind() const {
        glBindFramebuffer(GL_FRAMEBUFFER, mName);
    }
    
    const Size2D& GLFramebuffer::size() const {
        return mSize;
    }
    
    bool GLFramebuffer::isComplete() const {
        return glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
    }
    
    void GLFramebuffer::disableColorAttachments() {
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
    }
    
    void GLFramebuffer::attachTexture(const GLTexture2D& texture) {
        bind();
        texture.bind();
        glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texture.name(), 0);
        glDrawBuffer(GL_COLOR_ATTACHMENT0);
        glReadBuffer(GL_NONE);
    }
    
    void GLFramebuffer::attachTexture(const GLTextureCubemap& texture) {
        bind();
        texture.bind();
        glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texture.name(), 0);
        glDrawBuffer(GL_COLOR_ATTACHMENT0);
        glReadBuffer(GL_NONE);
    }
    
    void GLFramebuffer::attachTexture(const GLDepthTexture2D& texture) {
        bind();
        texture.bind();
        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, texture.name(), 0);
        disableColorAttachments();
    }
    
    void GLFramebuffer::attachTexture(const GLDepthTextureCubemap& texture) {
        bind();
        texture.bind();
        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, texture.name(), 0);
        disableColorAttachments();
    }
    
    void GLFramebuffer::attachTextureLayer(const GLDepthTexture2DArray& textures, uint16_t layer) {
        bind();
        textures.bind();
        glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, textures.name(), 0, layer);
        disableColorAttachments();
    }
    
}
