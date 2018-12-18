//
//  GLRenderbuffer.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 10.12.2017.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "GLRenderbuffer.hpp"
#include "StringUtils.hpp"

namespace EARenderer {

#pragma mark - Lifecycle

    GLRenderbuffer::GLRenderbuffer(const Size2D &size, GLenum internalFormat)
            :
            mSize(size) {
        if (size.width <= 0 || size.height <= 0) {
            throw std::invalid_argument("Renderbuffer's size must be greater than zero");
        }

        glGenRenderbuffers(1, &mName);
        bind();
        glRenderbufferStorage(GL_RENDERBUFFER, internalFormat, size.width, size.height);
    }

    GLRenderbuffer::~GLRenderbuffer() {
        glDeleteRenderbuffers(1, &mName);
    }

#pragma mark - Binding

    void GLRenderbuffer::bind() const {
        glBindRenderbuffer(GL_RENDERBUFFER, mName);
    }

#pragma mark - Getters

    const Size2D &GLRenderbuffer::size() const {
        return mSize;
    }

}
