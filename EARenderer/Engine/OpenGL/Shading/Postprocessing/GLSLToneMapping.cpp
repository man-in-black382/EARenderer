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
    GLProgram("FullScreenQuad.vert", "ToneMapping.frag", "")
    { }

#pragma mark - Lifecycle

    void GLSLToneMapping::setImage(const GLFloatTexture2D& image) {
        setUniformTexture(uint32_constant<ctcrc32("uImage")>, image);
    }

}
