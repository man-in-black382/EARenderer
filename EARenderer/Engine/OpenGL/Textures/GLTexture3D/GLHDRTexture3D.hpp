//
//  GLHDRTexture3D.hpp
//  EARenderer
//
//  Created by Pavel Muratov on 3/26/18.
//  Copyright © 2018 MPO. All rights reserved.
//

#ifndef GLHDRTexture3D_hpp
#define GLHDRTexture3D_hpp

#include "GLTexture3D.hpp"

namespace EARenderer {

    class GLHDRTexture3D : public GLTexture3D {
    public:
        GLHDRTexture3D(const Size2D &size, size_t depth);

        ~GLHDRTexture3D() = default;
    };

}

#endif /* GLHDRTexture3D_hpp */
