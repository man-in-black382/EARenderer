//
//  GLTexture2D.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 19.03.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "GLTexture2D.hpp"
#include "StringUtils.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <stdexcept>

namespace EARenderer {
    
#pragma mark - Lifecycle

    GLTexture2D::GLTexture2D()
    :
    GLTexture(GL_TEXTURE_2D)
    { }

    GLTexture2D::~GLTexture2D() { }

    void GLTexture2D::initialize(const Size2D& size, Filter filter, WrapMode wrapMode,
                                 GLint internalFormat, GLenum format, GLenum type, const void *pixelData)
    {
        if (size.width <= 0.0 || size.height <= 0.0) {
            throw std::invalid_argument("Texture size must not be zero");
        }

        mSize = size;

        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, size.width, size.height, 0, format, type, pixelData);

        setFilter(filter);
        setWrapMode(wrapMode);
    }

}
