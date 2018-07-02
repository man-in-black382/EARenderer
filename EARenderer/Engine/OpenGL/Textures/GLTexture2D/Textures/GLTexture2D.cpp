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

    GLNormalizedTexture2D::GLNormalizedTexture2D(const Size2D& size, NormalizedFormat format, void *data, Filter filter, WrapMode wrapMode) {
        GLTextureFormat f = glFormat(format);
        initialize(size, filter, wrapMode, f.internalFormat, f.inputPixelFormat, f.inputPixelType);
    }

    GLIntegerTexture2D::GLIntegerTexture2D(const Size2D& size, IntegerFormat format, void *data) {
        GLTextureFormat f = glFormat(format);
        initialize(size, Filter::None, WrapMode::ClampToEdge, f.internalFormat, f.inputPixelFormat, f.inputPixelType);
    }

    GLFloatTexture2D::GLFloatTexture2D(const Size2D& size, FloatFormat format, void *data, Filter filter, WrapMode wrapMode) {
        GLTextureFormat f = glFormat(format);
        initialize(size, filter, wrapMode, f.internalFormat, f.inputPixelFormat, f.inputPixelType);
    }

    GLDepthTexture2D::GLDepthTexture2D(const Size2D& size) {
        initialize(size, Filter::Bilinear, WrapMode::ClampToBorder, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_FLOAT);
        // Prevent oversampling by supplying 1.0 depth values when texture is sampled beyond [0.0; 1.0] range
        GLfloat borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    }
};
