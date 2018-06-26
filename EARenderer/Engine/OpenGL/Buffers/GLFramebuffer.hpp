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
#include "GLDepthTexture2D.hpp"
#include "GLDepthTextureCubemap.hpp"
#include "GLHDRTextureCubemap.hpp"
#include "GLDepthTexture2DArray.hpp"
#include "GLHDRTexture2DArray.hpp"
#include "GLDepthRenderbuffer.hpp"
#include "GLHDRTexture3D.hpp"
#include "GLLDRTexture3D.hpp"

#include "Size2D.hpp"
#include "Range.hpp"

#include <functional>
#include <unordered_set>
#include <vector>

namespace EARenderer {
    
    class GLFramebuffer: public GLNamedObject, public GLBindable {
    public:
        enum class Role { Source, Destination };

        enum class ColorAttachment {
            Attachment0, Attachment1, Attachment2, Attachment3,
            Attachment4, Attachment5, Attachment6, Attachment7,
            Attachment8, Attachment9, Attachment10, Attachment11,
            Attachment12, Attachment13, Attachment14, Attachment15
        };

    private:
        Role mRole;
        GLint mBindingPoint;
        Size2D mSize;
        GLViewport mViewport;
        GLint mMaximumColorAttachments;
        GLint mMaximumDrawBuffers;
        std::unordered_set<GLenum> mDrawBuffers;

        void obtainHardwareLimits();
        void attachTextureToDepthAttachment(const GLTexture& texture, uint16_t mipLevel = 0, int16_t layer = -1);
        void attachTextureToColorAttachment(const GLTexture& texture, ColorAttachment colorAttachment, uint16_t mipLevel = 0, int16_t layer = -1);
        GLenum glColorAttachment(ColorAttachment attachment) const;
        GLint glBindingPoint(Role role);
        
    public:
        GLFramebuffer(const Size2D& size, Role role = Role::Destination);
        GLFramebuffer(GLFramebuffer&& that) = default;
        GLFramebuffer& operator=(GLFramebuffer&& rhs) = default;
        ~GLFramebuffer() override;
        
        void bind() const override;
        
        const Size2D& size() const;
        bool isComplete() const;
        const GLViewport& viewport() const;

        void attachTexture(const GLDepthTexture2D& texture, uint16_t mipLevel = 0);

        void attachTexture(const GLTexture2D& texture,
                           ColorAttachment colorAttachment = ColorAttachment::Attachment0,
                           uint16_t mipLevel = 0);

        void attachTexture(const GLTextureCubemap& texture,
                           ColorAttachment colorAttachment = ColorAttachment::Attachment0,
                           uint16_t mipLevel = 0);

        void attachTexture(const GLHDRTexture2D& texture,
                           ColorAttachment colorAttachment = ColorAttachment::Attachment0,
                           uint16_t mipLevel = 0);

        void attachTexture(const GLDepthTextureCubemap& texture,
                           uint16_t mipLevel = 0);

        void attachTexture(const GLHDRTextureCubemap& texture,
                           ColorAttachment colorAttachment = ColorAttachment::Attachment0,
                           uint16_t mipLevel = 0);

        void attachTexture(const GLHDRTexture3D& texture,
                           ColorAttachment colorAttachment = ColorAttachment::Attachment0,
                           uint16_t mipLevel = 0);

        void attachTexture(const GLLDRTexture3D& texture,
                           ColorAttachment colorAttachment = ColorAttachment::Attachment0,
                           uint16_t mipLevel = 0);

        void attachTextureLayer(const GLDepthTexture2DArray& textures,
                                uint16_t layer,
                                uint16_t mipLevel = 0);

        void attachTextureLayer(const GLHDRTexture2DArray& textures,
                                uint16_t layer,
                                ColorAttachment colorAttachment = ColorAttachment::Attachment0,
                                uint16_t mipLevel = 0);

        void attachRenderbuffer(const GLDepthRenderbuffer& renderbuffer);
        
        void detachAllAttachments();

//        void blit();
    };
    
}

#endif /* GLFramebuffer_hpp */
