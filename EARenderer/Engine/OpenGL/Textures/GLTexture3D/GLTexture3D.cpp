//
//  GLTexture3D.cpp
//  EARenderer
//
//  Created by Pavel Muratov on 3/26/18.
//  Copyright © 2018 MPO. All rights reserved.
//

#include "GLTexture3D.hpp"

#include <stdexcept>
#include <OpenGL/gl3ext.h>

namespace EARenderer {

#pragma mark - Lifecycle

    GLTexture3D::GLTexture3D()
    :
    GLTexture(GL_TEXTURE_3D)
    { }

    GLTexture3D::~GLTexture3D() { }

    void GLTexture3D::initialize(const Size2D& size, size_t depth, Sampling::Filter filter, Sampling::WrapMode wrapMode, GLint internalFormat) {
        if (size.width <= 0.0 || size.height <= 0.0) {
            throw std::invalid_argument("3D texture size must not be zero");
        }

        if (depth == 0) {
            throw std::invalid_argument("3D texture must have non-zero depth");
        }

        mDepth = depth;
        mSize = size;

        glTexStorage3D(GL_TEXTURE_3D,
                       1, // No mipmaps (1 means that there is only one base image level)
                       internalFormat, // Internal format
                       size.width, // Width
                       size.height, // Height
                       (GLint)depth); // Depth of the 3D texture

        setFilter(filter);
        setWrapMode(wrapMode);
    }
    
    void GLTexture3D::initialize(const Size2D& size, Sampling::Filter filter, Sampling::WrapMode wrapMode, GLint internalFormat, GLenum format, GLenum type,
                                 const std::vector<void *> pixelData) {
        initialize(size, pixelData.size(), filter, wrapMode, internalFormat);

        for (size_t i = 0; i < pixelData.size(); i++) {
            void *pixels = pixelData[i];
            glTexSubImage3D(GL_TEXTURE_3D, 0, 0, 0, (GLint)i,
                            size.width, size.height, 1,
                            format, type, pixels);
        }
    }

#pragma mark - Getters

    size_t GLTexture3D::depth() const {
        return mDepth;
    }

}
