//
//  GLFramebuffer.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 29.04.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "GLFramebuffer.hpp"
#include "Macros.h"
#include "StringUtils.hpp"

#include <OpenGL/gl3.h>
#include <vector>
#include <stdexcept>

namespace EARenderer {
    
#pragma mark - Lifecycle
    
    GLFramebuffer::GLFramebuffer(const Size2D& size)
    :
    mBindingPoint(GL_FRAMEBUFFER),
    mSize(size),
    mViewport(Rect2D(size)),
    mAvailableAttachments({
        ColorAttachment::Attachment0, ColorAttachment::Attachment1, ColorAttachment::Attachment2, ColorAttachment::Attachment3,
        ColorAttachment::Attachment4, ColorAttachment::Attachment5, ColorAttachment::Attachment6, ColorAttachment::Attachment7,
        ColorAttachment::Attachment8, ColorAttachment::Attachment9, ColorAttachment::Attachment10, ColorAttachment::Attachment11,
        ColorAttachment::Attachment12, ColorAttachment::Attachment13, ColorAttachment::Attachment14, ColorAttachment::Attachment15
    })
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
        return glCheckFramebufferStatus(mBindingPoint) == GL_FRAMEBUFFER_COMPLETE;
    }
    
    const GLViewport& GLFramebuffer::viewport() const {
        return mViewport;
    }
    
#pragma mark - Binding
    
    void GLFramebuffer::bind() const {
        glBindFramebuffer(mBindingPoint, mName);
    }
    
#pragma mark - Private helpers
    
    void GLFramebuffer::obtainHardwareLimits() {
        glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &mMaximumColorAttachments);
        glGetIntegerv(GL_MAX_DRAW_BUFFERS, &mMaximumDrawBuffers);
    }

    void GLFramebuffer::setRequestedDrawBuffers() {
        std::array<GLenum, 16> drawBuffers;

        size_t i = 0;
        for (GLenum drawBuffer : mRequestedAttachments) {
            drawBuffers[i] = drawBuffer;
            i++;
        }

        std::sort(drawBuffers.begin(), drawBuffers.begin() + i);

        glDrawBuffers((GLsizei)mRequestedAttachments.size(), drawBuffers.data());
        glReadBuffer(GL_NONE);
    }
    
    void GLFramebuffer::attachTextureToDepthAttachment(const GLTexture& texture, uint16_t mipLevel, int16_t layer) {
        bind();
        texture.bind();
        
        if (layer == -1) {
            glFramebufferTexture(mBindingPoint, GL_DEPTH_ATTACHMENT, texture.name(), mipLevel);
        } else {
            glFramebufferTextureLayer(mBindingPoint, GL_DEPTH_ATTACHMENT, texture.name(), mipLevel, layer);
        }
        
        if (mRequestedAttachments.empty()) {
            glDrawBuffer(GL_NONE);
        }
        
        glReadBuffer(GL_NONE);
    }
    
    void GLFramebuffer::attachTextureToColorAttachment(const GLTexture& texture, ColorAttachment colorAttachment, uint16_t mipLevel, int16_t layer) {
        bind();
        texture.bind();

        GLenum attachment = glColorAttachment(colorAttachment);

        if (layer == -1) {
            glFramebufferTexture(mBindingPoint, attachment, texture.name(), mipLevel);
        } else {
            glFramebufferTextureLayer(mBindingPoint, attachment, texture.name(), mipLevel, layer);
        }

        mRequestedAttachments.insert(attachment);
        setRequestedDrawBuffers();
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
            default: return 0;
        }
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

    void GLFramebuffer::attachTexture(const GLLDRTexture3D& texture,
                                      ColorAttachment colorAttachment,
                                      uint16_t mipLevel)
    {
        attachTextureToColorAttachment(texture, colorAttachment, mipLevel);
    }

    void GLFramebuffer::attachTextureLayer(const GLHDRTexture2DArray& textures,
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
        bind();
        renderbuffer.bind();
        glFramebufferRenderbuffer(mBindingPoint, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, renderbuffer.name());
    }

    void GLFramebuffer::blit(GLFramebuffer::ColorAttachment sourceAttachment, const Rect2D& srcRect,
                             GLFramebuffer::ColorAttachment destinationAttachment, const Rect2D& dstRect,
                             bool useLinearFilter)
    {
        auto srcAttachment = glColorAttachment(sourceAttachment);
        auto dstAttachment = glColorAttachment(destinationAttachment);

        if (mRequestedAttachments.find(srcAttachment) == mRequestedAttachments.end()) {
            throw std::invalid_argument("Nothing is attached to passed source attachment");
        }

        if (mRequestedAttachments.find(dstAttachment) == mRequestedAttachments.end()) {
            throw std::invalid_argument("Nothing is attached to passed destination attachment");
        }

        bind();
        glReadBuffer(srcAttachment);
        glDrawBuffer(dstAttachment);
        glBlitFramebuffer(srcRect.origin.x, srcRect.origin.y, srcRect.origin.x + srcRect.size.width, srcRect.origin.y + srcRect.size.height,
                          dstRect.origin.x, dstRect.origin.y, dstRect.origin.x + dstRect.size.width, dstRect.origin.y + dstRect.size.height,
                          GL_COLOR_BUFFER_BIT, useLinearFilter ? GL_LINEAR : GL_NEAREST);
    }
    
}
