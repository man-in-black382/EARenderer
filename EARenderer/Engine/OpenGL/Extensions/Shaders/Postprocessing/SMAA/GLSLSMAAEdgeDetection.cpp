//
//  GLSLSMAAEdgeDetection.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 16.10.2018.
//  Copyright © 2018 MPO. All rights reserved.
//

#include "GLSLSMAAEdgeDetection.hpp"

namespace EARenderer {

#pragma mark - Lifecycle

    GLSLSMAAEdgeDetection::GLSLSMAAEdgeDetection()
            :
            GLProgram("SMAAEdgeDetection.vert", "SMAAEdgeDetection.frag", "") {
    }

#pragma mark - Setters

    void GLSLSMAAEdgeDetection::setImage(const GLFloatTexture2D<GLTexture::Float::RGBA16F> &image) {
        setUniformTexture(ctcrc32("uImage"), image);
    }

}
