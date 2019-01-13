//
// Created by Pavlo Muratov on 2018-12-24.
// Copyright (c) 2018 MPO. All rights reserved.
//

#include "GLUniformBuffer.hpp"
#include "StringUtils.hpp"

namespace EARenderer {

    GLUniformBuffer::GLUniformBuffer(const std::byte *data, size_t count)
            : GLBuffer<std::byte>(data, count, GL_UNIFORM_BUFFER, GL_DYNAMIC_DRAW, obtainMandatoryAlignment()) {

        GLint maximumSize = obtainMaximumSize();
        if (count > maximumSize) {
            throw std::invalid_argument(string_format("Exceeded the maximum UBO capacity. Requested: %d. Maximum: %d", count, maximumSize));
        }
    }

    GLUniformBuffer::GLUniformBuffer(size_t count) : GLUniformBuffer(nullptr, count) {}

    GLUniformBuffer::GLUniformBuffer() : GLUniformBuffer(obtainMaximumSize()) {}

    GLint GLUniformBuffer::obtainMandatoryAlignment() const {
        GLint alignment = 1;
        glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &alignment);
        return alignment;
    }

    GLint GLUniformBuffer::obtainMaximumSize() const {
        GLint size = 1;
        glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, &size);
        return size;
    }

}