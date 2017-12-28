//
//  GLDepthTextureCubemap.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 24.07.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#ifndef GLDepthTextureCubemap_hpp
#define GLDepthTextureCubemap_hpp

#include "GLTexture.hpp"

namespace EARenderer {
    
    class GLDepthTextureCubemap: public GLTexture {
    public:
        GLDepthTextureCubemap(const Size2D& size);
    };
    
}

#endif /* GLDepthTextureCubemap_hpp */
