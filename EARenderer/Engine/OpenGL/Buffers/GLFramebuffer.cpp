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
    mViewport(Rect2D(size))
    {
        ASSERT(size.width > 0, "Framebuffer width should be greater than 0");
        ASSERT(size.height > 0, "Framebuffer height should be greater than 0");
        
        glGenFramebuffers(1, &mName);
        obtainHardwareLimits();

        std::vector<GLenum> colorAttachments {
            GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3,
            GL_COLOR_ATTACHMENT4, GL_COLOR_ATTACHMENT5, GL_COLOR_ATTACHMENT6, GL_COLOR_ATTACHMENT7,
            GL_COLOR_ATTACHMENT8, GL_COLOR_ATTACHMENT9, GL_COLOR_ATTACHMENT10, GL_COLOR_ATTACHMENT11,
            GL_COLOR_ATTACHMENT12, GL_COLOR_ATTACHMENT13, GL_COLOR_ATTACHMENT14, GL_COLOR_ATTACHMENT15
        };

        mAvailableAttachments.insert(colorAttachments.begin(), colorAttachments.begin() + mMaximumColorAttachments);
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

    size_t GLFramebuffer::maximumColorAttachmentsCount() const {
        return mMaximumColorAttachments;
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
        if (texture.size().width > mSize.width || texture.size().height > mSize.height) {
            throw std::invalid_argument(string_format("Attempt to attach texture larger than framebuffer object. Texture size: %fx%f. FBO size: %fx%f", texture.size().width, texture.size().height, mSize.width, mSize.height));
        }
        
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
        if (texture.size().width > mSize.width || texture.size().height > mSize.height) {
            throw std::invalid_argument(string_format("Attempt to attach texture larger than framebuffer object. Texture size: %fx%f. FBO size: %fx%f", texture.size().width, texture.size().height, mSize.width, mSize.height));
        }

        if (mipLevel > texture.mipMapsCount()) {
            throw std::invalid_argument(string_format("Texture %d doesn't have %d mip level, therefore cannot attach it to the FBO.", texture.name(), mipLevel));
        }

        bind();
        texture.bind();

        GLenum glAttachment;

        if (colorAttachment == ColorAttachment::Automatic) {
            auto attachmentIt = mTextureAttachmentMap.find(texture.name());
            bool textureAlreadyAttached = attachmentIt != mTextureAttachmentMap.end();

            // If texture was already attached to the framebuffer
            // reuse attachment info and reattach (possible) different mip level
            if (textureAlreadyAttached) {

                if (attachmentIt->second.mipLevel == mipLevel) {
                    return;
                }

                glAttachment = attachmentIt->second.glColorAttachment;
                colorAttachment = attachmentIt->second.colorAttachment;

            } else {

                if (mRequestedAttachments.size() >= mMaximumColorAttachments) {
                    throw std::runtime_error(string_format("Exceeded maximum amount of color attachments (%d)", mMaximumColorAttachments));
                }

                auto freeAttachmentIt = mAvailableAttachments.begin();
                glAttachment = static_cast<std::underlying_type<ColorAttachment>::type>(*freeAttachmentIt);
                mAvailableAttachments.erase(freeAttachmentIt);
            }
        }
        else {
            glAttachment = static_cast<std::underlying_type<ColorAttachment>::type>(colorAttachment);
            mAvailableAttachments.erase(glAttachment);
        }

        AttachmentMetadata attachmentMetadata { colorAttachment, glAttachment, mipLevel, layer };
        mTextureAttachmentMap[texture.name()] = attachmentMetadata;

        if (layer == NotLayered) {
            glFramebufferTexture(mBindingPoint, glAttachment, texture.name(), mipLevel);
        } else {
            glFramebufferTextureLayer(mBindingPoint, glAttachment, texture.name(), mipLevel, layer);
        }

        mRequestedAttachments.insert(glAttachment);

        // FIXME: Delete this line, then ensure rendering works across the application
        setRequestedDrawBuffers();
    }
    
#pragma mark - Public

    // FIXME: Remove deprecated attachment functions

    void GLFramebuffer::attachTexture(const GLTextureCubemap& texture,
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

    void GLFramebuffer::detachTexture(const GLTexture& texture) {
        auto attachmentIt = mTextureAttachmentMap.find(texture.name());
        if (attachmentIt == mTextureAttachmentMap.end()) {
            throw std::invalid_argument(string_format("Texture %d was never attached to the framebuffer, therefore cannot detach it.", texture.name()));
        }

        AttachmentMetadata metadata = attachmentIt->second;

        if (metadata.layer == NotLayered) {
            glFramebufferTexture(mBindingPoint, metadata.glColorAttachment, 0, 0);
        } else {
            glFramebufferTextureLayer(mBindingPoint, metadata.glColorAttachment, 0, 0, 0);
        }

        mTextureAttachmentMap.erase(attachmentIt);
        mRequestedAttachments.erase(metadata.glColorAttachment);
        mAvailableAttachments.insert(metadata.glColorAttachment);
    }

    void GLFramebuffer::detachAllColorAttachments() {
        for (auto kvPair : mTextureAttachmentMap) {
            auto attachmentMetadata = kvPair.second;

            if (attachmentMetadata.layer == NotLayered) {
                glFramebufferTexture(mBindingPoint, attachmentMetadata.glColorAttachment, 0, 0);
            } else {
                glFramebufferTextureLayer(mBindingPoint, attachmentMetadata.glColorAttachment, 0, 0, 0);
            }

            mRequestedAttachments.erase(attachmentMetadata.glColorAttachment);
            mAvailableAttachments.insert(attachmentMetadata.glColorAttachment);
        }
        mTextureAttachmentMap.clear();
    }

    void GLFramebuffer::activateAllDrawBuffers() {
        setRequestedDrawBuffers();
    }

    void GLFramebuffer::replaceMipLevel(const GLTexture &texture, uint16_t newMipLevel) {
        auto attachmentIt = mTextureAttachmentMap.find(texture.name());
        if (attachmentIt == mTextureAttachmentMap.end()) {
            throw std::invalid_argument(string_format("Texture %d was never attached to the framebuffer, therefore cannot replace its mip level.", texture.name()));
        }

        attachTextureToColorAttachment(texture, attachmentIt->second.colorAttachment, newMipLevel);
    }

    void GLFramebuffer::blit(const GLTexture& fromTexture, const GLTexture& toTexture, bool useLinearFilter) {
        auto fromAttachmentIt = mTextureAttachmentMap.find(fromTexture.name());
        if (fromAttachmentIt == mTextureAttachmentMap.end()) {
            throw std::invalid_argument(string_format("Texture %d was never attached to the framebuffer, therefore cannot blit from it.", fromTexture.name()));
        }
        auto toAttachmentIt = mTextureAttachmentMap.find(toTexture.name());
        if (toAttachmentIt == mTextureAttachmentMap.end()) {
            throw std::invalid_argument(string_format("Texture %d was never attached to the framebuffer, therefore cannot blit to it.", toTexture.name()));
        }

        bind();
        glReadBuffer(fromAttachmentIt->second.glColorAttachment);
        glDrawBuffer(toAttachmentIt->second.glColorAttachment);

        Rect2D srcRect(fromTexture.size());
        Rect2D dstRect(toTexture.size());

        glBlitFramebuffer(srcRect.origin.x, srcRect.origin.y, srcRect.origin.x + srcRect.size.width, srcRect.origin.y + srcRect.size.height,
                          dstRect.origin.x, dstRect.origin.y, dstRect.origin.x + dstRect.size.width, dstRect.origin.y + dstRect.size.height,
                          GL_COLOR_BUFFER_BIT, useLinearFilter ? GL_LINEAR : GL_NEAREST);
    }

    void GLFramebuffer::clear(UnderlyingBuffer bufferMask) {
        using underlying = typename std::underlying_type<UnderlyingBuffer>::type;
        auto bitmask = static_cast<underlying>(bufferMask);
        bind();
        glClear(bitmask);
    }
    
}
