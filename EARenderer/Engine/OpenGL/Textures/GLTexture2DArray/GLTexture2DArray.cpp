//
//  GLTexture2DArray.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 02.08.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "GLTexture2DArray.hpp"

#include <OpenGL/gl3ext.h>
#include <stdexcept>

namespace EARenderer {
    
#pragma mark - Lifecycle
    
    GLTexture2DArray::GLTexture2DArray()
    :
    GLTexture(GL_TEXTURE_2D_ARRAY)
    { }

    GLTexture2DArray::~GLTexture2DArray() { }

    void GLTexture2DArray::initialize(const Size2D& size, size_t layers, Filter filter, WrapMode wrapMode, GLint internalFormat) {
        if (size.width <= 0.0 || size.height <= 0.0) {
            throw std::invalid_argument("2D texture array size must not be zero");
        }

        if (layers == 0) {
            throw std::invalid_argument("2D texture array must have non-zero layer count");
        }

        mSize = size;

        glTexStorage3D(GL_TEXTURE_2D_ARRAY,
                       1, // No mipmaps (1 means that there is only one base image level)
                       internalFormat, // Internal format
                       size.width, // Width
                       size.height, // Height
                       (GLint)layers); // Number of layers (elements, textures) in the array

        setFilter(filter);
        setWrapMode(wrapMode);
    }

    void GLTexture2DArray::initialize(const Size2D& size, Filter filter, WrapMode wrapMode, GLint internalFormat, GLenum format, GLenum type, const std::vector<void *> pixelData) {
        if (size.width <= 0.0 || size.height <= 0.0) {
            throw std::invalid_argument("2D texture array size must not be zero");
        }

        if (pixelData.size() == 0) {
            throw std::invalid_argument("2D texture array must have non-zero layer count");
        }

        mSize = size;

        for (size_t i = 0; i < pixelData.size(); i++) {
            void *pixels = pixelData[i];
            glTexImage3D(GL_TEXTURE_2D_ARRAY,
                         0, // Mip level
                         internalFormat,
                         size.width,
                         size.height,
                         (GLsizei)i, // Current layer
                         0, // Border. Must be 0.
                         format,
                         type,
                         pixels);
        }

        setFilter(filter);
        setWrapMode(wrapMode);
    }

#pragma mark - Getters

    size_t GLTexture2DArray::layers() const {
        return mLayers;
    }
    
}
