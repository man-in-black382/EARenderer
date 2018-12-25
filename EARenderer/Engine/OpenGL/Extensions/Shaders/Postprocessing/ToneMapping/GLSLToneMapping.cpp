//
//  GLSLToneMapping.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 28.06.2018.
//  Copyright © 2018 MPO. All rights reserved.
//

#include "GLSLToneMapping.hpp"

namespace EARenderer {

#pragma mark - Lifecycle

    GLSLToneMapping::GLSLToneMapping()
            :
            GLProgram("FullScreenQuad.vert", "ToneMapping.frag", "") {
    }

#pragma mark - Lifecycle

    void GLSLToneMapping::setImage(const GLFloatTexture2D<GLTexture::Float::RGBA16F> &image) {
        setUniformTexture(ctcrc32("uImage"), image);
    }

    void GLSLToneMapping::setExposure(const GLFloatTexture2D<GLTexture::Float::R16F> &exposure) {
        setUniformTexture(ctcrc32("uExposure"), exposure);
    }

}
