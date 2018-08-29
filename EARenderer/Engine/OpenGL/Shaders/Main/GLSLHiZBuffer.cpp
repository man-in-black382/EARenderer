//
//  GLSLHiZBuffer.cpp
//  EARenderer
//
//  Created by Pavel Muratov on 7/16/18.
//  Copyright © 2018 MPO. All rights reserved.
//

#include "GLSLHiZBuffer.hpp"

namespace EARenderer {

#pragma mark - Lifecycle

    GLSLHiZBuffer::GLSLHiZBuffer()
    :
    GLProgram("FullScreenQuad.vert", "HiZBuffer.frag", "")
    { }

#pragma mark - Setters

    void GLSLHiZBuffer::setTexture(const GLFloatTexture2D<GLTexture::Float::R32F> &texture) {
        setUniformTexture(uint32_constant<ctcrc32("uLinearDepthTexture")>, texture);
    }

    void GLSLHiZBuffer::setMipLevel(int8_t mipLevel) {
        glUniform1i(uniformByNameCRC32(uint32_constant<ctcrc32("uLOD")>).location(), mipLevel);
    }

}
