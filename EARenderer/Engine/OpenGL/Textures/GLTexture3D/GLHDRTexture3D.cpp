//
//  GLHDRTexture3D.cpp
//  EARenderer
//
//  Created by Pavel Muratov on 3/26/18.
//  Copyright © 2018 MPO. All rights reserved.
//

#include "GLHDRTexture3D.hpp"

namespace EARenderer {

#pragma mark - Lifecycle

    GLHDRTexture3D::GLHDRTexture3D(const Size2D& size, size_t depth) {
        initialize(size, depth, Filter::Bilinear, WrapMode::ClampToEdge, GL_RGBA32F);
    }

}
