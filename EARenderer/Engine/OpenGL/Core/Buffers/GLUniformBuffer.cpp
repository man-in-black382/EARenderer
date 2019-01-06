//
// Created by Pavlo Muratov on 2018-12-24.
// Copyright (c) 2018 MPO. All rights reserved.
//

#include "GLUniformBuffer.hpp"

namespace EARenderer {

    GLUniformBuffer::GLUniformBuffer(const std::byte *data, size_t count)
            : GLBuffer<std::byte>(data, count, GL_UNIFORM_BUFFER, GL_DYNAMIC_DRAW, obtainMandatoryAlignment()) {}

    GLint GLUniformBuffer::obtainMandatoryAlignment() const {
        GLint alignment = 1;
        glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &alignment);
        return alignment;
    }

}