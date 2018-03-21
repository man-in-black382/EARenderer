//
//  GLHDRTexture2D.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 09.10.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "GLHDRTexture2D.hpp"
#include "StringUtils.hpp"
#include "stb_image.h"

#include <stdexcept>

namespace EARenderer {
    
#pragma mark - Lifecycle
    
    GLHDRTexture2D::GLHDRTexture2D(const Size2D& size, Filter filter) {
        initialize(size, filter, WrapMode::ClampToEdge, GL_RGBA16F, GL_RGBA, GL_FLOAT);
    }
    
    GLHDRTexture2D::GLHDRTexture2D(const std::string& imagePath) {
        int32_t width = 0, height = 0, components = 0;
        stbi_set_flip_vertically_on_load(true);
        float *pixelData = stbi_loadf(imagePath.c_str(), &width, &height, &components, STBI_default);
        
        if (!pixelData) {
            throw std::runtime_error(string_format("Unable to read texture file: %s", imagePath.c_str()));
        }
        
        initialize(Size2D(width, height), Filter::Trilinear, WrapMode::Repeat, GL_RGB16F, GL_RGB, GL_FLOAT, pixelData);
        stbi_image_free(pixelData);
    };
    
}