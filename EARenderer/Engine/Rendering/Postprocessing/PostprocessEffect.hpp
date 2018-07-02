//
//  PostprocessingEffect.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 29.06.2018.
//  Copyright © 2018 MPO. All rights reserved.
//

#ifndef PostprocessingEffect_hpp
#define PostprocessingEffect_hpp

#include "GLFramebuffer.hpp"

#include <memory>

namespace EARenderer {

    class PostprocessEffect {
    protected:
        std::shared_ptr<GLFramebuffer> mSharedFramebuffer;

    public:
        PostprocessEffect(std::shared_ptr<GLFramebuffer> framebuffer);
    };

}


#endif /* PostprocessingEffect_hpp */
