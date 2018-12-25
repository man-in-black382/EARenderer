//
//  GLSLSurfelClusterLuminanceAveraging.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 24.03.2018.
//  Copyright © 2018 MPO. All rights reserved.
//

#include "GLSLSurfelClusterAveraging.hpp"

namespace EARenderer {

#pragma mark - Lifecycle

    GLSLSurfelClusterAveraging::GLSLSurfelClusterAveraging()
            : GLProgram("FullScreenQuad.vert", "SurfelClusterAveraging.frag", "") {
    }

#pragma mark - Setters

    void GLSLSurfelClusterAveraging::setSurfelClustersGBuffer(const GLIntegerTexture2D<GLTexture::Integer::R32UI> &gBuffer) {
        setUniformTexture(ctcrc32("uSurfelClustersGBuffer"), gBuffer);
    }

    void GLSLSurfelClusterAveraging::setSurfelsLuminaceMap(const GLFloatTexture2D<GLTexture::Float::R16F> &map) {
        setUniformTexture(ctcrc32("uSurfelsLuminanceMap"), map);
    }

}
