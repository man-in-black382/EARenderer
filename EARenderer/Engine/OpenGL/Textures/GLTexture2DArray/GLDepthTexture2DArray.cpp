//
//  GLTexture2DArray.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 02.08.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "GLDepthTexture2DArray.hpp"
 

#include <OpenGL/gl3ext.h>

namespace EARenderer {
    
#pragma mark - Lifecycle
    
    GLDepthTexture2DArray::GLDepthTexture2DArray(const Size2D& size, GLsizei layers)
    {
        initialize(size, layers, Filter::Bilinear, WrapMode::ClampToBorder, GL_DEPTH_COMPONENT24);
        
        // Prevent oversampling by supplying 1.0 depth values when texture is sampled beyond [0.0; 1.0] range
        GLfloat borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
        glTexParameterfv(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BORDER_COLOR, borderColor);
    }
    
}
