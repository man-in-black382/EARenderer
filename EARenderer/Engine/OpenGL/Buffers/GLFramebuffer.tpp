//
//  ghjk.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 05.08.2018.
//  Copyright © 2018 MPO. All rights reserved.
//

#ifndef GLFramebuffer_tpp
#define GLFramebuffer_tpp

namespace EARenderer {

    template<class... TexturePtrs>
    void GLFramebuffer::redirectRenderingToTexturesMip(const GLViewport& viewport, uint8_t mipLevel, UnderlyingBuffer buffersToClear, TexturePtrs... textures) {
        detachAllColorAttachments();
        attachTextures(mipLevel, *textures...);
        activateDrawBuffers(*textures...);
        viewport.apply();
        using type = std::underlying_type<UnderlyingBuffer>::type;
        glClear(static_cast<type>(UnderlyingBuffer::Color) | static_cast<type>(UnderlyingBuffer::Depth));
    }

    template<class... TexturePtrs>
    void GLFramebuffer::redirectRenderingToTexturesMip(uint8_t mipLevel, UnderlyingBuffer buffersToClear, TexturePtrs... textures) {
        GLViewport viewport(GLTexture::EstimatedMipSize(size(), mipLevel));
        redirectRenderingToTexturesMip(viewport, mipLevel, buffersToClear, textures...);
    }

    template<class... TexturePtrs>
    void GLFramebuffer::redirectRenderingToTextures(const GLViewport& viewport, UnderlyingBuffer buffersToClear, TexturePtrs... textures) {
        detachAllColorAttachments();
        attachTextures(0, *textures...);
        activateDrawBuffers(*textures...);
        viewport.apply();
        
        if (buffersToClear != UnderlyingBuffer::None) {
            using type = std::underlying_type<UnderlyingBuffer>::type;
            glClear(static_cast<type>(buffersToClear));
        }
    }

    template<class... TexturePtrs>
    void GLFramebuffer::redirectRenderingToTextures(UnderlyingBuffer buffersToClear, TexturePtrs... textures) {
        redirectRenderingToTextures(mViewport, buffersToClear, textures...);
    }

    template <class Texture>
    void GLFramebuffer::attachTextures(uint16_t mipLevel, const Texture& texture) {
        attachTextureToColorAttachment(texture, ColorAttachment::Automatic, mipLevel);
    }

    template <class Texture, class... Textures>
    void GLFramebuffer::attachTextures(uint16_t mipLevel, const Texture& head, const Textures&... tail) {
        attachTextureToColorAttachment(head, ColorAttachment::Automatic, mipLevel);
        attachTextures(mipLevel, tail...);
    }

    template<class Format, Format F>
    void GLFramebuffer::attachTexture(const GLTexture2D<Format, F>& texture,
                                      uint16_t mipLevel,
                                      ColorAttachment colorAttachment)
    {
        attachTextureToColorAttachment(texture, colorAttachment, mipLevel);
    }

    template<class Format, Format F>
    void GLFramebuffer::attachTexture(const GLTextureCubemapArray<Format, F>& texture,
                                      uint16_t mipLevel,
                                      int16_t layer,
                                      ColorAttachment colorAttachment)
    {
        attachTextureToColorAttachment(texture, colorAttachment, mipLevel, layer);
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

#endif 
