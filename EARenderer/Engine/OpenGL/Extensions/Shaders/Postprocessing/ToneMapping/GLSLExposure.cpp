//
//  GLSLExposure.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 18.08.2018.
//  Copyright © 2018 MPO. All rights reserved.
//

#include "GLSLExposure.hpp"

namespace EARenderer {

#pragma mark - Lifecycle

    GLSLExposure::GLSLExposure()
            :
            GLProgram("FullScreenQuad.vert", "Exposure.frag", "") {
    }

#pragma mark - Setters

    void GLSLExposure::setLuminanceHistogram(const GLFloatTexture2D<GLTexture::Float::R32F> &histogram) {
        setUniformTexture(ctcrc32("uHistogram"), histogram);
    }

}
