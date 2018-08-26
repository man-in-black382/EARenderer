//
//  PostprocessEffect.hpp
//  EARenderer
//
//  Created by Pavel Muratov on 8/22/18.
//  Copyright © 2018 MPO. All rights reserved.
//

#ifndef PostprocessEffect_hpp
#define PostprocessEffect_hpp

#include "GLFramebuffer.hpp"
#include "GLTexture2D.hpp"
#include "PostprocessTexturePool.hpp"

#include <memory>

namespace EARenderer {

    template <GLTexture::Float TextureFormat>
    class PostprocessEffect {
    protected:
        std::shared_ptr<GLFramebuffer> mFramebuffer;
        std::shared_ptr<PostprocessTexturePool<TextureFormat>> mTexturePool;

    public:
        PostprocessEffect(std::shared_ptr<GLFramebuffer> sharedFramebuffer, std::shared_ptr<PostprocessTexturePool<TextureFormat>> sharedTexturePool)
        :
        mFramebuffer(sharedFramebuffer),
        mTexturePool(sharedTexturePool)
        { }
    };

}

#endif /* PostprocessEffect_hpp */
