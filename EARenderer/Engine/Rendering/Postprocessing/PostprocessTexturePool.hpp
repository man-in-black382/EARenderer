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
        void replaceMipLevelsInFramebuffer(uint8_t mipLevel, TexturePtr texture);

        template<class TexturePtr, class... TexturePtrs>
        void replaceMipLevelsInFramebuffer(uint8_t mipLevel, TexturePtr head, TexturePtrs... tail);

    public:
        PostprocessTexturePool(const Size2D& resolution);

        std::shared_ptr<PostprocessTexture> claim();

        void putBack(std::shared_ptr<PostprocessTexture> texture);

        template<class... TexturePtrs>
        void redirectRenderingToTexturesMip(uint8_t mipLevel, TexturePtrs... textures);

        template<class... TexturePtrs>
        void redirectRenderingToTextures(TexturePtrs... textures);

        void useInternalDepthBuffer();

        void useExternalDepthBuffer(std::shared_ptr<GLDepthTexture2D> depthBuffer);
    };

}

#include "PostprocessTexturePool.tpp"

#endif /* TexturePool_hpp */
