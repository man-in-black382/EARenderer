//
//  GLHDRTexture2D.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 09.10.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#ifndef GLHDRTexture2D_hpp
#define GLHDRTexture2D_hpp

#include "GLTexture2D.hpp"

#include <vector>

namespace EARenderer {
    
    class GLHDRTexture2D: public GLTexture2D {
    public:
        GLHDRTexture2D(const Size2D& size, Filter filter = Filter::Trilinear);
        GLHDRTexture2D(const std::string& imagePath);
        GLHDRTexture2D(const std::vector<float>, const Size2D& size);
        ~GLHDRTexture2D() = default;
    };
    
}

#endif /* GLHDRTexture2D_hpp */
