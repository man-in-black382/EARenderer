//
//  GLLDRTexture2D.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 18.03.2018.
//  Copyright © 2018 MPO. All rights reserved.
//

#include "GLLDRTexture2D.hpp"
#include "GLLDRTexture2DSampler.hpp"
#include "StringUtils.hpp"
#include "stb_image.h"

namespace EARenderer {

#pragma mark - Lifecycle

    GLLDRTexture2D::GLLDRTexture2D(const Size2D& size, Filter filter) {
        initialize(size, filter, WrapMode::ClampToEdge, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE);
    }

    GLLDRTexture2D::GLLDRTexture2D(const std::string& imagePath) {
        int32_t width = 0;
        int32_t height = 0;
        int32_t components = 0;
        stbi_set_flip_vertically_on_load(true);
        stbi_uc *pixelData = stbi_load(imagePath.c_str(), &width, &height, &components, STBI_rgb_alpha);

        if (!pixelData) {
            throw std::runtime_error(string_format("Unable to read texture file: %s", imagePath.c_str()));
        }

        GLint texComponents;
        switch (components) {
            case 1: texComponents = GL_RED; break;
            case 2: texComponents = GL_RG; break;
            case 3: texComponents = GL_RGB; break;
            default: texComponents = GL_RGBA; break;
        }

        initialize(Size2D(width, height), Filter::Trilinear, WrapMode::Repeat, texComponents, GL_RGBA, GL_UNSIGNED_BYTE, pixelData);
        stbi_image_free(pixelData);        
    }

#pragma mark - Sampling

    void GLLDRTexture2D::sampleTexels(int32_t mipLevel, SamplerClosure samplerClosure) const {
        bind();
        samplerClosure(GLLDRTexture2DSampler(*this, mipLevel));
    }

}
