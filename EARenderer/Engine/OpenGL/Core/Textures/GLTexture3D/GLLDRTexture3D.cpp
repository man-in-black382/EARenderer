//
//  GLLDRTexture3D.cpp
//  EARenderer
//
//  Created by Pavel Muratov on 6/8/18.
//  Copyright © 2018 MPO. All rights reserved.
//

#include "GLLDRTexture3D.hpp"

namespace EARenderer {

#pragma mark - Lifecycle

    GLLDRTexture3D::GLLDRTexture3D(const Size2D &size, size_t depth) {
        initialize(size, depth, Sampling::Filter::None, Sampling::WrapMode::ClampToEdge, GL_RGBA32UI);
    }

}
