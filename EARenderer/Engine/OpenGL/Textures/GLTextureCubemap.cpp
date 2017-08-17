//
//  GLTextureCubemap.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 21.04.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "GLTextureCubemap.hpp"
#include "Macros.h"
#include "stb_image.h"

namespace EARenderer {
    
#pragma mark - Lifecycle
    
    GLTextureCubemap::GLTextureCubemap(const Size2D& size)
    :
    GLTexture(size, GL_TEXTURE_CUBE_MAP, GL_NEAREST, GL_NEAREST, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE)
    {
        ASSERT(size.width > 0, "Cubemap texture width should be greater than 0");
        ASSERT(size.height > 0, "Cubemap texture height should be greater than 0");
        
        for(GLuint i = 0; i < 6; i++) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, size.width,
                         size.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        }
    }
    
    GLTextureCubemap::GLTextureCubemap(const std::string& rightImagePath,
                                       const std::string& leftImagePath,
                                       const std::string& topImagePath,
                                       const std::string& bottomImagePath,
                                       const std::string& frontImagePath,
                                       const std::string& backImagePath)
    :
    GLTexture(GL_TEXTURE_CUBE_MAP, GL_NEAREST, GL_NEAREST, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE)
    {
        int32_t width = 0;
        int32_t height = 0;
        int32_t components = 0;
        
        std::string paths[6] = {
            rightImagePath, leftImagePath, topImagePath,
            bottomImagePath, frontImagePath, backImagePath
        };
        
        for(GLuint i = 0; i < 6; i++) {
            stbi_uc *pixelData = stbi_load(paths[i].c_str(), &width, &height, &components, STBI_rgb_alpha);
            ASSERT(pixelData, "Unable to read texture file: " << rightImagePath);
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, width,
                         height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixelData);
            stbi_image_free(pixelData);
        }
        
        mSize = Size2D(width, height);
    }
 
}
