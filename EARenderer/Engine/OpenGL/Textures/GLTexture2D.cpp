//
//  GLTexture2D.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 19.03.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "GLTexture2D.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "Macros.h"

namespace EARenderer {
    
#pragma mark - Lifecycle
    
    GLTexture2D::GLTexture2D(const Size2D& size)
    :
    GLTexture(size, GL_TEXTURE_2D, GL_NEAREST, GL_NEAREST, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE)
    {
        ASSERT(size.width > 0, "Texture width should be greater than 0");
        ASSERT(size.height > 0, "Texture height should be greater than 0");

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, size.width, size.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    }
    
    GLTexture2D::GLTexture2D(const std::string& fileName)
    :
    GLTexture(GL_TEXTURE_2D, GL_NEAREST, GL_NEAREST, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE)
    {
        int32_t width = 0;
        int32_t height = 0;
        int32_t components = 0;
        stbi_uc *pixelData = stbi_load(fileName.c_str(), &width, &height, &components, STBI_rgb_alpha);
        
        ASSERT(pixelData, "Unable to read texture file: " << fileName);
        
        mSize = Size2D(width, height);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixelData);
        stbi_image_free(pixelData);
    };
 
}
