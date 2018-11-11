//
//  GLFramebuffer.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 29.04.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#ifndef GLFramebuffer_hpp
#define GLFramebuffer_hpp

#include "GLNamedObject.hpp"
#include "GLBindable.hpp"
#include "GLViewport.hpp"

#include "GLTexture2D.hpp"
#include "GLTextureCubemap.hpp"
#include "GLDepthTextureCubemap.hpp"
#include "GLHDRTextureCubemap.hpp"
#include "GLDepthTexture2DArray.hpp"
#include "GLHDRTexture2DArray.hpp"
#include "GLDepthRenderbuffer.hpp"
#include "GLHDRTexture3D.hpp"
#include "GLLDRTexture3D.hpp"
#include "GLTextureCubemapArray.hpp"

#include "Size2D.hpp"
#include "Range.hpp"
#include "StringUtils.hpp"
#include "BitwiseEnum.hpp"

#include <functional>
#include <unordered_set>
#include <unordered_map>
#include <vector>

namespace EARenderer {

    class GLFramebuffer: public GLNamedObject, public GLBindable {
    public:
        enum class ColorAttachment {
            Automatic = 0,
            Attachment0 = GL_COLOR_ATTACHMENT0, Attachment1 = GL_COLOR_ATTACHMENT1, Attachment2 = GL_COLOR_ATTACHMENT2, Attachment3 = GL_COLOR_ATTACHMENT3,
            Attachment4 = GL_COLOR_ATTACHMENT4, Attachment5 = GL_COLOR_ATTACHMENT5, Attachment6 = GL_COLOR_ATTACHMENT6, Attachment7 = GL_COLOR_ATTACHMENT7,
            Attachment8 = GL_COLOR_ATTACHMENT8, Attachment9 = GL_COLOR_ATTACHMENT9, Attachment10 = GL_COLOR_ATTACHMENT10, Attachment11 = GL_COLOR_ATTACHMENT11,
            Attachment12 = GL_COLOR_ATTACHMENT12, Attachment13 = GL_COLOR_ATTACHMENT13, Attachment14 = GL_COLOR_ATTACHMENT14, Attachment15 = GL_COLOR_ATTACHMENT15
        };

        enum class UnderlyingBuffer {
            None = 0, Color = GL_COLOR_BUFFER_BIT, Depth = GL_DEPTH_BUFFER_BIT, Stencil = GL_STENCIL_BUFFER_BIT
        };

    private:
        constexpr static int16_t NotLayered = -1;

        struct AttachmentMetadata {
            ColorAttachment colorAttachment = ColorAttachment::Automatic;
            GLenum glColorAttachment = GL_COLOR_ATTACHMENT0;
            uint16_t mipLevel = 0;
            int16_t layer = NotLayered;
        };

        GLint mBindingPoint;
        Size2D mSize;
        GLViewport mViewport;
        GLint mMaximumColorAttachments;
        GLint mMaximumDrawBuffers;
        std::unordered_set<GLenum> mRequestedAttachments;
        std::unordered_set<GLenum> mAvailableAttachments;
        std::unordered_map<GLint, AttachmentMetadata> mTextureAttachmentMap;
        std::vector<GLenum> mDrawBuffers;

        void obtainHardwareLimits();
        void setRequestedDrawBuffers();
        void attachTextureToDepthAttachment(const GLTexture& texture, uint16_t mipLevel = 0, int16_t layer = -1);
        void attachTextureToColorAttachment(const GLTexture& texture, ColorAttachment colorAttachment, uint16_t mipLevel = 0, int16_t layer = -1);

    public:

#pragma mark - Lifecycle

        GLFramebuffer(const Size2D& size);

        GLFramebuffer(GLFramebuffer&& that) = default;

        GLFramebuffer& operator=(GLFramebuffer&& rhs) = default;

        ~GLFramebuffer() override;

#pragma mark - Getters

        void bind() const override;
        
        const Size2D& size() const;

        const GLViewport& viewport() const;

        bool isComplete() const;

        size_t maximumColorAttachmentsCount() const;

#pragma mark - Attachments

        template <class Texture>
        void attachTextures(uint16_t mipLevel, const Texture& texture);

        template <class Texture, class... Textures>
        void attachTextures(uint16_t mipLevel, const Texture& head, const Textures&... tail);

        template<class Format, Format F>
        void attachTexture(const GLTexture2D<Format, F>& texture,
                           uint16_t mipLevel = 0,
                           ColorAttachment colorAttachment = ColorAttachment::Automatic);

        template<class Format, Format F>
        void attachTexture(const GLTextureCubemapArray<Format, F>& texture,
                           uint16_t mipLevel = 0,
                           int16_t layer = NotLayered,
                           ColorAttachment colorAttachment = ColorAttachment::Automatic);

        void attachDepthTexture(const GLDepthTextureCubemapArray& texture, uint16_t mipLevel = 0, int16_t layer = NotLayered);

        void attachDepthTexture(const GLDepthTexture2D& texture, uint16_t mipLevel = 0);

        void attachRenderbuffer(const GLDepthRenderbuffer& renderbuffer);

        void detachTexture(const GLTexture& texture);

        void detachAllColorAttachments();

        template<class Texture>
        void activateDrawBuffers(const Texture& texture);

        template<class Texture, class... Textures>
        void activateDrawBuffers(const Texture& head, const Textures&... tail);

        void activateAllDrawBuffers();

        void blit(const GLTexture& fromTexture, const GLTexture& toTexture, bool useLinearFilter = true);

        void clear(UnderlyingBuffer bufferMask);

#pragma mark - Convenience

        /**
         Replaces all currenlty attached textures with the passed ones and uses a specified mip map,
         applies a specified non-standard viewport, activates draw buffers for all passed textures,
         clears all necessary buffers

         @param viewport non-standart viewport
         @param mipLevel mip level to attach
         @param buffersToClear mask to specify which buffers (depth, color and/or stencil) to clear
         @param textures textures to which rendering will be redirected
         */
        template<class... TexturePtrs>
        void redirectRenderingToTexturesMip(const GLViewport& viewport, uint8_t mipLevel, UnderlyingBuffer buffersToClear, TexturePtrs... textures);

        /**
         Replaces all currenlty attached textures with the passed ones and uses a specified mip map,
         applies default viewport, activates draw buffers for all passed textures,
         clears all necessary buffers

         @param mipLevel mip level to attach
         @param buffersToClear mask to specify which buffers (depth, color and/or stencil) to clear
         @param textures textures to which rendering will be redirected
         */
        template<class... TexturePtrs>
        void redirectRenderingToTexturesMip(uint8_t mipLevel, UnderlyingBuffer buffersToClear, TexturePtrs... textures);

        /**
         Replaces all currenlty attached textures with the passed ones,
         applies a specified non-standard viewport, activates draw buffers for all passed textures,
         clears all necessary buffers

         @param viewport non-standart viewport
         @param buffersToClear mask to specify which buffers (depth, color and/or stencil) to clear
         @param textures textures to which rendering will be redirected
         */
        template<class... TexturePtrs>
        void redirectRenderingToTextures(const GLViewport& viewport, UnderlyingBuffer buffersToClear, TexturePtrs... textures);

        /**
         Replaces all currenlty attached textures with the passed ones,
         applies default viewport, activates draw buffers for all passed textures,
         clears all necessary buffers

         @param buffersToClear mask to specify which buffers (depth, color and/or stencil) to clear
         @param textures textures to which rendering will be redirected
         */
        template<class... TexturePtrs>
        void redirectRenderingToTextures(UnderlyingBuffer buffersToClear, TexturePtrs... textures);

        // FIXME: Deprecated attachment functions

        void attachTexture(const GLTextureCubemap& texture,
                           ColorAttachment colorAttachment = ColorAttachment::Automatic,
                           uint16_t mipLevel = 0);

        void attachTexture(const GLDepthTextureCubemap& texture,
                           uint16_t mipLevel = 0);

        void attachTexture(const GLHDRTextureCubemap& texture,
                           ColorAttachment colorAttachment = ColorAttachment::Automatic,
                           uint16_t mipLevel = 0);

        void attachTexture(const GLHDRTexture3D& texture,
                           ColorAttachment colorAttachment = ColorAttachment::Automatic,
                           uint16_t mipLevel = 0);

        void attachTexture(const GLLDRTexture3D& texture,
                           ColorAttachment colorAttachment = ColorAttachment::Automatic,
                           uint16_t mipLevel = 0);

        void attachTextureLayer(const GLDepthTexture2DArray& textures,
                                uint16_t layer,
                                uint16_t mipLevel = 0);

        void attachTextureLayer(const GLHDRTexture2DArray& textures,
                                uint16_t layer,
                                ColorAttachment colorAttachment = ColorAttachment::Automatic,
                                uint16_t mipLevel = 0);
    };
    
}

ENABLE_BITMASK_OPERATORS(EARenderer::GLFramebuffer::UnderlyingBuffer);

#include "GLFramebuffer.tpp"

#endif /* GLFramebuffer_hpp */
