//
//  GLDepthTextureCubemap.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 24.07.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "GLDepthTextureCubemap.hpp"
#include "Macros.h"

namespace EARenderer {
    
#pragma mark - Lifecycle
    
    GLDepthTextureCubemap::GLDepthTextureCubemap(const Size2D& size)
    :
    GLTexture(size, GL_TEXTURE_CUBE_MAP, GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_BORDER, GL_CLAMP_TO_BORDER)
    {
        ASSERT(size.width > 0, "Depth texture width should be greater than 0");
        ASSERT(size.height > 0, "Depth texture height should be greater than 0");

        for(GLuint i = 0; i < 6; i++) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, size.width,
                         size.height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
        }
    }
    
}
