//
//  GLSLLuminanceRange.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 18.08.2018.
//  Copyright © 2018 MPO. All rights reserved.
//

#include "GLSLLuminanceRange.hpp"

namespace EARenderer {

#pragma mark - Lifecycle

    GLSLLuminanceRange::GLSLLuminanceRange()
    :
    GLProgram("FullScreenQuad.vert", "LuminanceRange.frag", "")
    { }

#pragma mark - Setters

    void GLSLLuminanceRange::setLuminance(const GLFloatTexture2D<GLTexture::Float::RG16F>& luminance) {
        setUniformTexture(uint32_constant<ctcrc32("uLuminance")>, luminance);
    }

    void GLSLLuminanceRange::setMipLevel(int8_t mipLevel) {
        glUniform1i(uniformByNameCRC32(uint32_constant<ctcrc32("uLOD")>).location(), mipLevel);
    }

}
