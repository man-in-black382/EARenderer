//
//  GLDepthRenderbuffer.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 10.12.2017.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "GLDepthRenderbuffer.hpp"

namespace EARenderer {

#pragma mark - Lifecycle

    GLDepthRenderbuffer::GLDepthRenderbuffer(const Size2D &size)
            :
            GLRenderbuffer(size, GL_DEPTH_COMPONENT24) {
    }

}
