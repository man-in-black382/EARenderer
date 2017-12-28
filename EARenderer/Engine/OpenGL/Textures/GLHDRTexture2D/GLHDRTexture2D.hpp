//
//  GLHDRTexture2D.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 09.10.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#ifndef GLHDRTexture2D_hpp
#define GLHDRTexture2D_hpp

#include "GLTexture.hpp"

namespace EARenderer {
    
    class GLHDRTexture2D: public GLTexture {
    public:
        GLHDRTexture2D(const Size2D& size);
        GLHDRTexture2D(const std::string& fileName);
    };
    
}

#endif /* GLHDRTexture2D_hpp */
