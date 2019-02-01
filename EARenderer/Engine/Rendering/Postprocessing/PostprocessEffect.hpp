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

    class PostprocessEffect {
    protected:
        GLFramebuffer *mFramebuffer;
        PostprocessTexturePool *mTexturePool;

    public:
        PostprocessEffect(GLFramebuffer *sharedFramebuffer, PostprocessTexturePool *sharedTexturePool)
                : mFramebuffer(sharedFramebuffer), mTexturePool(sharedTexturePool) {}
    };

}

#endif /* PostprocessEffect_hpp */
