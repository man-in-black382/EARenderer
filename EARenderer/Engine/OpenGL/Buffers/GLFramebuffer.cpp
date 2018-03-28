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
        
        if (mDrawBuffers.empty()) {
            glDrawBuffer(GL_NONE);
        }
        
        glReadBuffer(GL_NONE);
    }
    
    void GLFramebuffer::attachTextureToColorAttachment(const GLTexture& texture, ColorAttachment colorAttachment, uint16_t mipLevel, int16_t layer) {
        bind();
        texture.bind();

        GLenum attachment = glColorAttachment(colorAttachment);

        if (layer == -1) {
            glFramebufferTexture(GL_FRAMEBUFFER, attachment, texture.name(), mipLevel);
        } else {
            glFramebufferTextureLayer(GL_FRAMEBUFFER, attachment, texture.name(), mipLevel, layer);
        }

        mDrawBuffers.insert(attachment);

        std::array<GLenum, 16> drawBuffers;
        size_t i = 0;
        for (GLenum drawBuffer : mDrawBuffers) {
            drawBuffers[i] = drawBuffer;
        }

        glDrawBuffers((GLsizei)mDrawBuffers.size(), drawBuffers.data());
        glReadBuffer(GL_NONE);
    }

    GLenum GLFramebuffer::glColorAttachment(ColorAttachment attachment) const {
        switch (attachment) {
            case ColorAttachment::Attachment0: return GL_COLOR_ATTACHMENT0;
            case ColorAttachment::Attachment1: return GL_COLOR_ATTACHMENT1;
            case ColorAttachment::Attachment2: return GL_COLOR_ATTACHMENT2;
            case ColorAttachment::Attachment3: return GL_COLOR_ATTACHMENT3;
            case ColorAttachment::Attachment4: return GL_COLOR_ATTACHMENT4;
            case ColorAttachment::Attachment5: return GL_COLOR_ATTACHMENT5;
            case ColorAttachment::Attachment6: return GL_COLOR_ATTACHMENT6;
            case ColorAttachment::Attachment7: return GL_COLOR_ATTACHMENT7;
            case ColorAttachment::Attachment8: return GL_COLOR_ATTACHMENT8;
            case ColorAttachment::Attachment9: return GL_COLOR_ATTACHMENT9;
            case ColorAttachment::Attachment10: return GL_COLOR_ATTACHMENT10;
            case ColorAttachment::Attachment11: return GL_COLOR_ATTACHMENT11;
            case ColorAttachment::Attachment12: return GL_COLOR_ATTACHMENT12;
            case ColorAttachment::Attachment13: return GL_COLOR_ATTACHMENT13;
            case ColorAttachment::Attachment14: return GL_COLOR_ATTACHMENT14;
            case ColorAttachment::Attachment15: return GL_COLOR_ATTACHMENT15;
        }
    }
    
    void GLFramebuffer::detachAllAttachments() {
        bind();
        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, 0, 0);
        glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, 0, 0, 0);
        glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, 0, 0);
        glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, 0, 0, 0);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, 0);
        mDrawBuffers.clear();
    }
    
#pragma mark - Public
    
    void GLFramebuffer::attachTexture(const GLTexture2D& texture,
                                      ColorAttachment colorAttachment,
                                      uint16_t mipLevel)
    {
        attachTextureToColorAttachment(texture, colorAttachment, mipLevel);
    }

    void GLFramebuffer::attachTexture(const GLTextureCubemap& texture,
                                      ColorAttachment colorAttachment,
                                      uint16_t mipLevel)
    {
        attachTextureToColorAttachment(texture, colorAttachment, mipLevel);
    }

    void GLFramebuffer::attachTexture(const GLHDRTexture2D& texture,
                                      ColorAttachment colorAttachment,
                                      uint16_t mipLevel)
    {
        attachTextureToColorAttachment(texture, colorAttachment, mipLevel);
    }

    void GLFramebuffer::attachTexture(const GLHDRTextureCubemap& texture,
                                      ColorAttachment colorAttachment,
                                      uint16_t mipLevel)
    {
        attachTextureToColorAttachment(texture, colorAttachment, mipLevel);
    }

    void GLFramebuffer::attachTexture(const GLDepthTexture2D& texture,
                                      uint16_t mipLevel)
    {
        attachTextureToDepthAttachment(texture, mipLevel);
    }

    void GLFramebuffer::attachTexture(const GLDepthTextureCubemap& texture,
                                      uint16_t mipLevel)
    {
        attachTextureToDepthAttachment(texture, mipLevel);
    }

    void GLFramebuffer::attachTexture(const GLHDRTexture3D& texture,
                                      ColorAttachment colorAttachment,
                                      uint16_t mipLevel)
    {
        attachTextureToColorAttachment(texture, colorAttachment, mipLevel);
    }

    void GLFramebuffer::attachTextureLayer(const GLTexture2DArray& textures,
                                           uint16_t layer,
                                           ColorAttachment colorAttachment,
                                           uint16_t mipLevel)
    {
        attachTextureToColorAttachment(textures, colorAttachment, mipLevel, layer);
    }

    void GLFramebuffer::attachTextureLayer(const GLDepthTexture2DArray& textures,
                                           uint16_t layer,
                                           uint16_t mipLevel)
    {
        attachTextureToDepthAttachment(textures, mipLevel, layer);
    }
    
    void GLFramebuffer::attachRenderbuffer(const GLDepthRenderbuffer& renderbuffer) {
        renderbuffer.bind();
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, renderbuffer.name());
    }
    
}
