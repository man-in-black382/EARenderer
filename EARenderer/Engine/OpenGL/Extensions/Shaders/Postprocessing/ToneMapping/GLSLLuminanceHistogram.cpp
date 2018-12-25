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
            GLProgram("LuminanceHistogram.vert", "LuminanceHistogram.frag", "") {
    }

#pragma mark - Setters

    void GLSLLuminanceHistogram::setLuminance(const GLFloatTexture2D<GLTexture::Float::RG16F> &luminance) {
        setUniformTexture(ctcrc32("uLuminance"), luminance);
        glUniform1i(uniformByNameCRC32(ctcrc32("uLuminanceMaxLOD")).location(), (GLint) luminance.mipMapCount());
    }

    void GLSLLuminanceHistogram::setHistogramWidth(size_t width) {
        glUniform1i(uniformByNameCRC32(ctcrc32("uHistogramWidth")).location(), (GLint) width);
    }

}
