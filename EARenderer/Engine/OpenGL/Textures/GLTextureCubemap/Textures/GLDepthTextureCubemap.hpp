//
//  GLDepthTextureCubemap.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 24.07.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#ifndef GLDepthTextureCubemap_hpp
#define GLDepthTextureCubemap_hpp

#include "GLTextureCubemap.hpp"

namespace EARenderer {
    
    class GLDepthTextureCubemap: public GLTextureCubemap {
    public:
        GLDepthTextureCubemap(const Size2D& size);
        ~GLDepthTextureCubemap() = default;
    };
    
}

#endif /* GLDepthTextureCubemap_hpp */
