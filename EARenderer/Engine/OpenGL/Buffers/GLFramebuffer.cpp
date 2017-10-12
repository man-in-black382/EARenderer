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
    
    void GLFramebuffer::attachTextureToDepthAttachment(const GLTexture& texture, int16_t layer) {
        bind();
        texture.bind();
        
        if (layer == -1) {
            glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, texture.name(), 0);
        } else {
            glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, texture.name(), 0, layer);
        }
        
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
    }
    
    void GLFramebuffer::attachTextureToColorAttachment0(const GLTexture& texture, int16_t layer) {
        bind();
        texture.bind();
        
        if (layer == -1) {
            glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texture.name(), 0);
        } else {
            glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texture.name(), 0, layer);
        }
        
        glDrawBuffer(GL_COLOR_ATTACHMENT0);
        glReadBuffer(GL_NONE);
    }
    
#pragma mark - Public
    
    void GLFramebuffer::attachTexture(const GLTexture2D& texture)                                   { attachTextureToColorAttachment0(texture); }
    void GLFramebuffer::attachTexture(const GLTextureCubemap& texture)                              { attachTextureToColorAttachment0(texture); }
    void GLFramebuffer::attachTexture(const GLHDRTexture2D& texture)                                { attachTextureToColorAttachment0(texture); }
    void GLFramebuffer::attachTexture(const GLHDRTextureCubemap& texture)                           { attachTextureToColorAttachment0(texture); }
    void GLFramebuffer::attachTexture(const GLDepthTexture2D& texture)                              { attachTextureToDepthAttachment(texture); }
    void GLFramebuffer::attachTexture(const GLDepthTextureCubemap& texture)                         { attachTextureToDepthAttachment(texture); }
    void GLFramebuffer::attachTextureLayer(const GLTexture2DArray& textures, uint16_t layer)        { attachTextureToColorAttachment0(textures, layer); }
    void GLFramebuffer::attachTextureLayer(const GLDepthTexture2DArray& textures, uint16_t layer)   { attachTextureToDepthAttachment(textures, layer); }
    
    void GLFramebuffer::attachMipMapsOfTexture(const GLHDRTextureCubemap& texture, uint8_t mipMapCount) {
        ASSERT(mipMapCount <= mMaximumColorAttachments, "Cannot attach " << mipMapCount << " mip maps. Current hardware only supports " << mMaximumColorAttachments << " color attachments.");
        ASSERT(mipMapCount <= mMaximumDrawBuffers, "Cannot attach " << mipMapCount << " mip maps. Current hardware only supports " << mMaximumDrawBuffers  << " draw buffers.");
        
        bind();
        texture.bind();
        
        std::vector<GLenum> colorAttachment;
        for (uint8_t mipLevel = 0; mipLevel < mipMapCount; ++mipLevel) {
            glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + mipLevel, texture.name(), mipLevel);
            colorAttachment.emplace_back(GL_COLOR_ATTACHMENT0 + mipLevel);
        }
        
        glDrawBuffers(static_cast<GLsizei>(colorAttachment.size()), colorAttachment.data());
        glReadBuffer(GL_NONE);
    }
    
}
