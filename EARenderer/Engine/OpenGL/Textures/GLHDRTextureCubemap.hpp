//
//  GLHDRTextureCubemap.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 09.10.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#ifndef GLHDRTextureCubemap_hpp
#define GLHDRTextureCubemap_hpp

#include "GLTexture.hpp"

namespace EARenderer {
    
    class GLHDRTextureCubemap: public GLTexture {
    public:
        GLHDRTextureCubemap(const Size2D& size, bool generateMipmaps = false);
    };
    
}

#endif /* GLHDRTextureCubemap_hpp */
