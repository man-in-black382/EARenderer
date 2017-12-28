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
    
    GLDepthTexture2D::GLDepthTexture2D(const Size2D& size)
    :
    GLTexture(size, GL_TEXTURE_2D, GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_BORDER, GL_CLAMP_TO_BORDER)
    {
        ASSERT(size.width > 0, "Depth texture width should be greater than 0");
        ASSERT(size.height > 0, "Depth texture height should be greater than 0");

        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, size.width, size.height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
        
        // Prevent oversampling by supplying 1.0 depth values when texture is sampled beyond [0.0; 1.0] range
        GLfloat borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    }
    
}
