//
//  TexturePool.hpp
//  EARenderer
//
//  Created by Pavel Muratov on 7/3/18.
//  Copyright © 2018 MPO. All rights reserved.
//

#ifndef TexturePool_tpp
#define TexturePool_tpp

namespace EARenderer {

    template<class TexturePtr>
    void PostprocessTexturePool::replaceMipLevelsInFramebuffer(uint8_t mipLevel, TexturePtr texture) {
        mFramebuffer.replaceMipLevel(*texture, mipLevel);
    }

    template<class TexturePtr, class... TexturePtrs>
    void PostprocessTexturePool::replaceMipLevelsInFramebuffer(uint8_t mipLevel, TexturePtr head, TexturePtrs... tail) {
        mFramebuffer.replaceMipLevel(*head, mipLevel);
        replaceMipLevelsInFramebuffer(mipLevel, tail...);
    }

    template<class... TexturePtrs>
    void PostprocessTexturePool::redirectRenderingToTexturesMip(const GLViewport& viewport, uint8_t mipLevel, TexturePtrs... textures) {
        mFramebuffer.detachAllColorAttachments();
        mFramebuffer.attachTextures(mipLevel, *textures...);
        mFramebuffer.activateDrawBuffers(*textures...);
        viewport.apply();
        mFramebuffer.clear(GLFramebuffer::UnderlyingBuffer::Color | GLFramebuffer::UnderlyingBuffer::Depth);
    }

    template<class... TexturePtrs>
    void PostprocessTexturePool::redirectRenderingToTexturesMip(uint8_t mipLevel, TexturePtrs... textures) {
        GLViewport viewport(GLTexture::EstimatedMipSize(mFramebuffer.size(), mipLevel));
        redirectRenderingToTexturesMip(viewport, mipLevel, textures...);
    }

    template<class... TexturePtrs>
    void PostprocessTexturePool::redirectRenderingToTextures(const GLViewport& viewport, TexturePtrs... textures) {
        mFramebuffer.detachAllColorAttachments();
        mFramebuffer.attachTextures(0, *textures...);
        mFramebuffer.activateDrawBuffers(*textures...);
        viewport.apply();
        mFramebuffer.clear(GLFramebuffer::UnderlyingBuffer::Color | GLFramebuffer::UnderlyingBuffer::Depth);
    }

    template<class... TexturePtrs>
    void PostprocessTexturePool::redirectRenderingToTextures(TexturePtrs... textures) {
        redirectRenderingToTextures(mFramebuffer.viewport(), textures...);
    }

}

#endif /* TexturePool_hpp */
