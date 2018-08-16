//
//  GLSLLuminanceHistogram.cpp
//  EARenderer
//
//  Created by Pavel Muratov on 8/16/18.
//  Copyright © 2018 MPO. All rights reserved.
//

#include "GLSLLuminanceHistogram.hpp"

namespace EARenderer {

#pragma mark - Lifecycle

    GLSLLuminanceHistogram::GLSLLuminanceHistogram()
    :
    GLProgram("LuminanceHistogram.vert", "LuminanceHistogram.frag", "")
    { }

#pragma mark - Setters

    void GLSLLuminanceHistogram::setImage(const GLFloatTexture2D<GLTexture::Float::RGBA16F>& image) {
        setUniformTexture(uint32_constant<ctcrc32("uImage")>, image);
    }

    void GLSLLuminanceHistogram::setHistogramWidth(size_t width) {
        glUniform1i(uniformByNameCRC32(uint32_constant<ctcrc32("uHistogramWidth")>).location(), (GLint)width);
    }

}
