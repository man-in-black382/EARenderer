//
//  TexturePool.cpp
//  EARenderer
//
//  Created by Pavel Muratov on 7/3/18.
//  Copyright © 2018 MPO. All rights reserved.
//

#include "PostprocessTexturePool.hpp"

#include <stdexcept>

namespace EARenderer {

    PostprocessTexturePool::PostprocessTexturePool(const Size2D& resolution)
    :
    mFramebuffer(resolution),
    mDepthRenderbuffer(resolution)
    {
        for (size_t i = 0; i < mFramebuffer.maximumColorAttachmentsCount(); i++) {
            auto texture = std::make_shared<GLHDRTexture2D>(resolution);
            mFreeTextures.insert(texture);
            mFramebuffer.attachTexture(*texture);
        }
        mFramebuffer.attachRenderbuffer(mDepthRenderbuffer);
    }

    std::shared_ptr<GLHDRTexture2D> PostprocessTexturePool::claim() {
        if (mFreeTextures.empty()) {
            throw std::runtime_error("Texture pool is empty: all textures have been claimed");
        }
        auto textureIt = mFreeTextures.begin();
        auto texture = *textureIt;
        mFreeTextures.erase(textureIt);
        mClaimedTextures.insert(texture);
        return texture;
    }

    void PostprocessTexturePool::putBack(std::shared_ptr<GLHDRTexture2D> texture) {
        auto textureIt = mClaimedTextures.find(texture);
        if (textureIt == mClaimedTextures.end()) {
            throw std::invalid_argument("Attempt to put a texture in a pool that was never in it in the first place");
        }
        mClaimedTextures.erase(textureIt);
        mFreeTextures.insert(texture);
    }

}
