//
//  ghjk.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 05.08.2018.
//  Copyright © 2018 MPO. All rights reserved.
//

#ifndef ghjk_hpp
#define ghjk_hpp

namespace EARenderer {

    template <class Texture>
    void GLFramebuffer::attachTextures(uint16_t mipLevel, const Texture& texture) {
        attachTextureToColorAttachment(texture, ColorAttachment::Automatic, mipLevel);
    }

    template <class Texture, class... Textures>
    void GLFramebuffer::attachTextures(uint16_t mipLevel, const Texture& head, const Textures&... tail) {
        attachTextureToColorAttachment(head, ColorAttachment::Automatic, mipLevel);
        attachTextures(mipLevel, tail...);
    }

    template <GLTexture::Normalized Format>
    void GLFramebuffer::attachTexture(const GLNormalizedTexture2D<Format>& texture,
                                      ColorAttachment colorAttachment,
                                      uint16_t mipLevel)
    {
        attachTextureToColorAttachment(texture, colorAttachment, mipLevel);
    }

    template <GLTexture::Float Format>
    void GLFramebuffer::attachTexture(const GLFloatTexture2D<Format>& texture,
                                      ColorAttachment colorAttachment,
                                      uint16_t mipLevel)
    {
        attachTextureToColorAttachment(texture, colorAttachment, mipLevel);
    }

    template<GLTexture::Integer Format>
    void GLFramebuffer::attachTexture(const GLIntegerTexture2D<Format>& texture,
                                      ColorAttachment colorAttachment)
    {
        attachTextureToColorAttachment(texture, colorAttachment, 0);
    }

    template<class Texture>
    void GLFramebuffer::activateDrawBuffers(const Texture& texture) {
        auto attachmentIt = mTextureAttachmentMap.find(texture.name());
        if (attachmentIt == mTextureAttachmentMap.end()) {
            throw std::invalid_argument(string_format("Texture %d was never attached to the framebuffer, therefore cannot redirect rendering to it.", texture.name()));
        }

        bind();
        mDrawBuffers.emplace_back(attachmentIt->second.glColorAttachment);
        glDrawBuffers(GLsizei(mDrawBuffers.size()), mDrawBuffers.data());
        mDrawBuffers.clear();
    }

    template<class Texture, class... Textures>
    void GLFramebuffer::activateDrawBuffers(const Texture& head, const Textures&... tail) {
        auto attachmentIt = mTextureAttachmentMap.find(head.name());
        if (attachmentIt == mTextureAttachmentMap.end()) {
            throw std::invalid_argument(string_format("Texture %d was never attached to the framebuffer, therefore cannot redirect rendering to it.", head.name()));
        }

        mDrawBuffers.emplace_back(attachmentIt->second.glColorAttachment);
        activateDrawBuffers(tail...);
    }

}

#endif /* ghjk_hpp */
