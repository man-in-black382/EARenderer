//
//  GLFramebufferPool.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 01.07.2018.
//  Copyright © 2018 MPO. All rights reserved.
//

#ifndef GLFramebufferPool_hpp
#define GLFramebufferPool_hpp

#include "GLFramebuffer.hpp"

namespace EARenderer {

    class GLFramebufferPool {
    private:
        std::vector<GLFramebuffer> mFramebuffers;
    };

}

#endif /* GLFramebufferPool_hpp */
