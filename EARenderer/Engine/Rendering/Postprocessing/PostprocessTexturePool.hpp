//
//  TexturePool.hpp
//  EARenderer
//
//  Created by Pavel Muratov on 7/3/18.
//  Copyright © 2018 MPO. All rights reserved.
//

#ifndef TexturePool_hpp
#define TexturePool_hpp

#include "GLTexture2D.hpp"
#include "GLFramebuffer.hpp"
#include "GLDepthRenderbuffer.hpp"

#include <unordered_set>
#include <memory>

namespace EARenderer {

    class PostprocessTexturePool {
    public:
        using PostprocessTexture = GLFloatTexture2D<GLTexture::Float::RGBA16F>;

    private:
        using TexturePointerSet = std::unordered_set<std::shared_ptr<PostprocessTexture>>;

        GLFramebuffer mFramebuffer;
        GLDepthRenderbuffer mDepthRenderbuffer;
        TexturePointerSet mFreeTextures;
        TexturePointerSet mClaimedTextures;

        template<class TexturePtr>
        void replaceMipLevelsInFramebuffer(TexturePtr texture, uint8_t mipLevel) {
            mFramebuffer.replaceMipLevel(*texture, mipLevel);
        }

        template<class TexturePtr, class... TexturePtrs>
        void replaceMipLevelsInFramebuffer(TexturePtr head, TexturePtrs... tail, uint8_t mipLevel) {
            mFramebuffer.replaceMipLevel(*head, mipLevel);
            replaceMipLevelsInFramebuffer(tail...);
        }

    public:
        PostprocessTexturePool(const Size2D& resolution);

        const GLFramebuffer& framebuffer() const;

        std::shared_ptr<PostprocessTexture> claim();

        void putBack(std::shared_ptr<PostprocessTexture> texture);

        void redirectRenderingToTextureMip(std::shared_ptr<PostprocessTexture> texture, uint8_t mipLevel);

        template<class... TexturePtrs>
        void redirectRenderingToTexturesMip(TexturePtrs... textures, uint8_t mipLevel) {
            redirectRenderingToTexturesMip(textures..., mipLevel);
            mFramebuffer.activateDrawBuffers(*textures...);
            GLViewport(GLTexture::EstimatedMipSize(mFramebuffer.size(), mipLevel)).apply();
            mFramebuffer.clear(GLFramebuffer::UnderlyingBuffer::Color | GLFramebuffer::UnderlyingBuffer::Depth);
        }

        template<class... TexturePtrs>
        void redirectRenderingToTextures(TexturePtrs... textures) {
            mFramebuffer.activateDrawBuffers(*textures...);
            mFramebuffer.viewport().apply();
            mFramebuffer.clear(GLFramebuffer::UnderlyingBuffer::Color | GLFramebuffer::UnderlyingBuffer::Depth);
        }
    };

}

#endif /* TexturePool_hpp */
