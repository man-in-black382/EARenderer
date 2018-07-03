//
//  PostprocessingEffect.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 29.06.2018.
//  Copyright © 2018 MPO. All rights reserved.
//

#ifndef PostprocessingEffect_hpp
#define PostprocessingEffect_hpp

#include "PostprocessTexturePool.hpp"

#include <memory>

namespace EARenderer {

    class PostprocessEffect {
    protected:
        std::shared_ptr<PostprocessTexturePool> mTexturePool;

    public:
        PostprocessEffect(std::shared_ptr<PostprocessTexturePool> texturePool);
    };

}


#endif /* PostprocessingEffect_hpp */
