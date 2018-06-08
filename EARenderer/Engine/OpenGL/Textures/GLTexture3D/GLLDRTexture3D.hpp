//
//  GLLDRTexture3D.hpp
//  EARenderer
//
//  Created by Pavel Muratov on 6/8/18.
//  Copyright © 2018 MPO. All rights reserved.
//

#ifndef GLLDRTexture3D_hpp
#define GLLDRTexture3D_hpp

#include "GLTexture3D.hpp"

namespace EARenderer {

    class GLLDRTexture3D: public GLTexture3D {
        public:
        GLLDRTexture3D(const Size2D& size, size_t depth);
        ~GLLDRTexture3D() = default;
    };

}

#endif /* GLLDRTexture3D_hpp */
