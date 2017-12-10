//
//  GLFramebuffer.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 29.04.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "GLFramebuffer.hpp"
#include "Macros.h"

#include <OpenGL/gl3.h>
#include <vector>

namespace EARenderer {
    
#pragma mark - Lifecycle
    
    GLFramebuffer::GLFramebuffer(const Size2D& size)
    :
    mSize(size),
    mViewport(Rect2D(size))
    {
        ASSERT(size.width > 0, "Framebuffer width should be greater than 0");
        ASSERT(size.height > 0, "Framebuffer height should be greater than 0");
        
        glGenFramebuffers(1, &mName);
        obtainHardwareLimits();
    }
    
    GLFramebuffer::~GLFramebuffer() {
        glDeleteFramebuffers(1, &mName);
    }
    
#pragma mark - Getters
    
    const Size2D& GLFramebuffer::size() const {
        return mSize;
    }
    
    bool GLFramebuffer::isComplete() const {
        return glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
    }
    
    const GLViewport& GLFramebuffer::viewport() const {
        return mViewport;
    }
    
#pragma mark - Binding
    
    void GLFramebuffer::bind() const {
        glBindFramebuffer(GL_FRAMEBUFFER, mName);
    }
    
#pragma mark - Private helpers
    
    void GLFramebuffer::obtainHardwareLimits() {
        glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &mMaximumColorAttachments);
        glGetIntegerv(GL_MAX_DRAW_BUFFERS, &mMaximumDrawBuffers);
    }
    
    void GLFramebuffer::attachTextureToDepthAttachment(const GLTexture& texture, uint16_t mipLevel, int16_t layer) {
        bind();
        texture.bind();
        
        if (layer == -1) {
            glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, texture.name(), mipLevel);
        } else {
            glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, texture.name(), mipLevel, layer);
        }
        
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
    }
    
    void GLFramebuffer::attachTextureToColorAttachment0(const GLTexture& texture, uint16_t mipLevel, int16_t layer) {
        bind();
        texture.bind();
        
        if (layer == -1) {
            glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texture.name(), mipLevel);
        } else {
            glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texture.name(), mipLevel, layer);
        }
        
        glDrawBuffer(GL_COLOR_ATTACHMENT0);
        glReadBuffer(GL_NONE);
    }
    
    void GLFramebuffer::detachAllAttachments() {
        bind();
        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, 0, 0);
        glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, 0, 0, 0);
        glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, 0, 0);
        glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, 0, 0, 0);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, 0);
    }
    
#pragma mark - Public
    
    void GLFramebuffer::attachTexture(const GLTexture2D& texture, uint16_t mipLevel) { attachTextureToColorAttachment0(texture, mipLevel); }
    void GLFramebuffer::attachTexture(const GLTextureCubemap& texture, uint16_t mipLevel) { attachTextureToColorAttachment0(texture, mipLevel); }
    void GLFramebuffer::attachTexture(const GLHDRTexture2D& texture, uint16_t mipLevel) { attachTextureToColorAttachment0(texture, mipLevel); }
    void GLFramebuffer::attachTexture(const GLHDRTextureCubemap& texture, uint16_t mipLevel) { attachTextureToColorAttachment0(texture, mipLevel); }
    void GLFramebuffer::attachTexture(const GLDepthTexture2D& texture, uint16_t mipLevel) { attachTextureToDepthAttachment(texture, mipLevel); }
    void GLFramebuffer::attachTexture(const GLDepthTextureCubemap& texture, uint16_t mipLevel) { attachTextureToDepthAttachment(texture, mipLevel); }
    void GLFramebuffer::attachTextureLayer(const GLTexture2DArray& textures, uint16_t layer, uint16_t mipLevel) { attachTextureToColorAttachment0(textures, mipLevel, layer); }
    void GLFramebuffer::attachTextureLayer(const GLDepthTexture2DArray& textures, uint16_t layer, uint16_t mipLevel) { attachTextureToDepthAttachment(textures, mipLevel, layer); }
    
    void GLFramebuffer::attachRenderbuffer(const GLDepthRenderbuffer& renderbuffer) {
        renderbuffer.bind();
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, renderbuffer.name());
    }
    
}
