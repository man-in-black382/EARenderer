//
//  GLDepthTexture2D.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 30.04.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "GLDepthTexture2D.hpp"
#include "Macros.h"

namespace EARenderer {

#pragma mark - Lifecycle

    GLDepthTexture2D::GLDepthTexture2D(const Size2D& size)
    {
        initialize(size, Filter::Bilinear, WrapMode::ClampToBorder, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_FLOAT);

        // Prevent oversampling by supplying 1.0 depth values when texture is sampled beyond [0.0; 1.0] range
        GLfloat borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    }
    
}
