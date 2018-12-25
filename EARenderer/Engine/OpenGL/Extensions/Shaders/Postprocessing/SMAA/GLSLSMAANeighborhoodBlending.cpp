//
//  GLSLSMAANeightborhoodBlending.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 22.10.2018.
//  Copyright © 2018 MPO. All rights reserved.
//

#include "GLSLSMAANeighborhoodBlending.hpp"

namespace EARenderer {

#pragma mark - Lifecycle

    GLSLSMAANeighborhoodBlending::GLSLSMAANeighborhoodBlending()
            :
            GLProgram("SMAANeighborhoodBlending.vert", "SMAANeighborhoodBlending.frag", "") {
    }

#pragma mark - Setters

    void GLSLSMAANeighborhoodBlending::setImage(const GLFloatTexture2D<GLTexture::Float::RGBA16F> &image) {
        setUniformTexture(ctcrc32("uImage"), image);
    }

    void GLSLSMAANeighborhoodBlending::setBlendingWeights(const GLFloatTexture2D<GLTexture::Float::RGBA16F> &weightsTexture) {
        setUniformTexture(ctcrc32("uBlendingWeights"), weightsTexture);
    }

}
