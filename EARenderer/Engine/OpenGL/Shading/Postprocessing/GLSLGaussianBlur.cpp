//
//  GLSLGaussianBlur.cpp
//  EARenderer
//
//  Created by Pavel Muratov on 6/13/18.
//  Copyright © 2018 MPO. All rights reserved.
//

#include "GLSLGaussianBlur.hpp"

namespace EARenderer {

#pragma mark - Lifecycle

    GLSLGaussianBlur::GLSLGaussianBlur()
    :
    GLProgram("FullScreenQuad.vert", "GaussianBlur.frag", "")
    { }

#pragma mark - Lifecycle

    void GLSLGaussianBlur::setBlurDirection(BlurDirection direction) {

    }

}
