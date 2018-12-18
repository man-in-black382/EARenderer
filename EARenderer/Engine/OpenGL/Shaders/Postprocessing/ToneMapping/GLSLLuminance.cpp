//
//  GLSLLuminance.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 18.08.2018.
//  Copyright © 2018 MPO. All rights reserved.
//

#include "GLSLLuminance.hpp"

namespace EARenderer {

#pragma mark - Lifecycle

    GLSLLuminance::GLSLLuminance()
            :
            GLProgram("FullScreenQuad.vert", "Luminance.frag", "") {
    }

#pragma mark - Setters

    void GLSLLuminance::setImage(const GLFloatTexture2D<GLTexture::Float::RGBA16F> &image) {
        setUniformTexture(uint32_constant<ctcrc32("uImage")>, image);
    }

}
