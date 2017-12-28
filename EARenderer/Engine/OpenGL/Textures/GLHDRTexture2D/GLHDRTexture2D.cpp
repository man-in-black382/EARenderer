//
//  GLHDRTexture2D.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 09.10.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "GLHDRTexture2D.hpp"

#include "stb_image.h"
#include "Macros.h"

namespace EARenderer {
    
#pragma mark - Lifecycle
    
    GLHDRTexture2D::GLHDRTexture2D(const Size2D& size)
    :
    GLTexture(size, GL_TEXTURE_2D, GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE)
    {
        ASSERT(size.width > 0, "Texture width should be greater than 0");
        ASSERT(size.height > 0, "Texture height should be greater than 0");
        
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, size.width, size.height, 0, GL_RGB, GL_FLOAT, nullptr);
    }
    
    GLHDRTexture2D::GLHDRTexture2D(const std::string& fileName)
    :
    GLTexture(GL_TEXTURE_2D, GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE)
    {
        int32_t width = 0, height = 0, components = 0;
        stbi_set_flip_vertically_on_load(true);
        float *pixelData = stbi_loadf(fileName.c_str(), &width, &height, &components, STBI_default);
        
        ASSERT(pixelData, "Unable to read texture file: " << fileName);
        
        mSize = Size2D(width, height);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, pixelData);
        stbi_image_free(pixelData);
    };
    
}
