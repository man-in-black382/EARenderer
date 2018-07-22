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
#include "GLHDRTexture2D.hpp"
#include "GLTextureCubemap.hpp"
#include "GLDepthTextureCubemap.hpp"
#include "GLHDRTextureCubemap.hpp"
#include "GLDepthTexture2DArray.hpp"
#include "GLHDRTexture2DArray.hpp"
#include "GLDepthRenderbuffer.hpp"
#include "GLHDRTexture3D.hpp"
#include "GLLDRTexture3D.hpp"

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
            Automatic,
            Attachment0, Attachment1, Attachment2, Attachment3,
            Attachment4, Attachment5, Attachment6, Attachment7,
            Attachment8, Attachment9, Attachment10, Attachment11,
            Attachment12, Attachment13, Attachment14, Attachment15
        };

        enum class UnderlyingBuffer {
            Color = GL_COLOR_BUFFER_BIT, Depth = GL_DEPTH_BUFFER_BIT, Stencil = GL_STENCIL_BUFFER_BIT
        };

    private:
        GLint mBindingPoint;
        Size2D mSize;
        GLViewport mViewport;
        GLint mMaximumColorAttachments;
        GLint mMaximumDrawBuffers;
        std::unordered_set<GLenum> mRequestedAttachments;
        std::unordered_set<ColorAttachment> mAvailableAttachments;
        std::unordered_map<GLint, GLenum> mTextureAttachmentMap;
        std::vector<GLenum> mDrawBuffers;

        void obtainHardwareLimits();
        void setRequestedDrawBuffers();
        void attachTextureToDepthAttachment(const GLTexture& texture, uint16_t mipLevel = 0, int16_t layer = -1);
        void attachTextureToColorAttachment(const GLTexture& texture, ColorAttachment colorAttachment, uint16_t mipLevel = 0, int16_t layer = -1);
        GLenum glColorAttachment(ColorAttachment attachment) const;

    public:
        GLFramebuffer(const Size2D& size);
        GLFramebuffer(GLFramebuffer&& that) = default;
        GLFramebuffer& operator=(GLFramebuffer&& rhs) = default;
        ~GLFramebuffer() override;
        
        void bind() const override;
        
        const Size2D& size() const;
        const GLViewport& viewport() const;
        bool isComplete() const;
        size_t maximumColorAttachmentsCount() const;

        template <GLTexture::Normalized Format>
        void attachTexture(const GLNormalizedTexture2D<Format>& texture,
                           ColorAttachment colorAttachment = ColorAttachment::Automatic,
                           uint16_t mipLevel = 0)
        {
            attachTextureToColorAttachment(texture, colorAttachment, mipLevel);
        }

        template <GLTexture::Float Format>
        void attachTexture(const GLFloatTexture2D<Format>& texture,
                           ColorAttachment colorAttachment = ColorAttachment::Automatic,
                           uint16_t mipLevel = 0)
        {
            attachTextureToColorAttachment(texture, colorAttachment, mipLevel);
        }

        template<GLTexture::Integer Format>
        void attachTexture(const GLIntegerTexture2D<Format>& texture,
                           ColorAttachment colorAttachment = ColorAttachment::Automatic)
        {
            attachTextureToColorAttachment(texture, colorAttachment, 0);
        }

        void attachTexture(const GLDepthTexture2D& texture, uint16_t mipLevel = 0) {
            attachTextureToDepthAttachment(texture, mipLevel);
        }

        // FIXME: Deprecated attachment functions
//        void attachTexture(const GLTexture2D& texture,
//                           ColorAttachment colorAttachment = ColorAttachment::Automatic,
//                           uint16_t mipLevel = 0);

        void attachTexture(const GLTextureCubemap& texture,
                           ColorAttachment colorAttachment = ColorAttachment::Automatic,
                           uint16_t mipLevel = 0);

//        void attachTexture(const GLHDRTexture2D& texture,
//                           ColorAttachment colorAttachment = ColorAttachment::Automatic,
//                           uint16_t mipLevel = 0);

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

        void attachRenderbuffer(const GLDepthRenderbuffer& renderbuffer);

        template<class Texture>
        void activateDrawBuffers(const Texture& texture) {
            auto attachmentIt = mTextureAttachmentMap.find(texture.name());
            if (attachmentIt == mTextureAttachmentMap.end()) {
                throw std::invalid_argument(string_format("Texture %d was never attached to the framebuffer, therefore cannot redirect rendering to it.", texture.name()));
            }

            bind();
            mDrawBuffers.emplace_back(attachmentIt->second);
            glDrawBuffers(GLsizei(mDrawBuffers.size()), mDrawBuffers.data());
            mDrawBuffers.clear();
        }

        template<class Texture, class... Textures>
        void activateDrawBuffers(const Texture& head, const Textures&... tail) {
            auto attachmentIt = mTextureAttachmentMap.find(head.name());
            if (attachmentIt == mTextureAttachmentMap.end()) {
                throw std::invalid_argument(string_format("Texture %d was never attached to the framebuffer, therefore cannot redirect rendering to it.", head.name()));
            }

            mDrawBuffers.emplace_back(attachmentIt->second);
            activateDrawBuffers(tail...);
        }

        void blit(ColorAttachment sourceAttachment, const Rect2D& srcRect,
                  ColorAttachment destinationAttachment, const Rect2D& dstRect,
                  bool useLinearFilter = true);

        void blit(const GLTexture& fromTexture, const GLTexture& toTexture, bool useLinearFilter = true);

        void clear(UnderlyingBuffer bufferMask);
    };
    
}

ENABLE_BITMASK_OPERATORS(EARenderer::GLFramebuffer::UnderlyingBuffer);

#endif /* GLFramebuffer_hpp */
