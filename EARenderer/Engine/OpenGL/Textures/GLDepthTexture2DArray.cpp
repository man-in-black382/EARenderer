//
//  GLTexture2DArray.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 02.08.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "GLDepthTexture2DArray.hpp"
#include "Macros.h"

#include <OpenGL/gl3ext.h>

namespace EARenderer {
    
#pragma mark - Lifecycle
    
    GLDepthTexture2DArray::GLDepthTexture2DArray(const Size2D& size, GLsizei layers)
    :
    GLLayeredTexture(size, layers, GL_TEXTURE_2D_ARRAY, GL_NEAREST, GL_NEAREST, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE)
    {
        ASSERT(size.width > 0, "Texture 2d array width should be greater than 0");
        ASSERT(size.height > 0, "Texture 2d array height should be greater than 0");
        
        glTexStorage3D(GL_TEXTURE_2D_ARRAY,
                       1, // No mipmaps (1 means that there is only one base image level)
                       GL_DEPTH_COMPONENT24, // Internal format
                       size.width, // Width
                       size.height, // Height
                       layers); // Number of layers (elements, textures) in the array

        // Prevent oversampling by supplying 1.0 depth values when texture is sampled beyond [0.0; 1.0] range
        GLfloat borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
        glTexParameterfv(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BORDER_COLOR, borderColor);
    }
    
}
