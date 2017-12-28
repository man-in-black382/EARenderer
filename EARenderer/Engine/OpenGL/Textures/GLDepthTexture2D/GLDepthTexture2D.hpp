//
//  GLDepthTexture2D.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 30.04.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#ifndef GLDepthTexture2D_hpp
#define GLDepthTexture2D_hpp

#include "GLTexture.hpp"

namespace EARenderer {
    
    class GLDepthTexture2D: public GLTexture {
    public:
        GLDepthTexture2D(const Size2D& size);
    };
    
}

#endif /* GLDepthTexture2D_hpp */
