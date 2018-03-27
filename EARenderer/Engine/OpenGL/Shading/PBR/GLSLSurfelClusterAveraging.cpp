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
    :
    GLProgram("FullScreenQuad.vert", "SurfelClusterAveraging.frag", "")
    { }

#pragma mark - Setters

    void GLSLSurfelClusterAveraging::setSurfelClustersGBuffer(const GLLDRTexture2D& gBuffer) {
        setUniformTexture(uint32_constant<ctcrc32("uSurfelClustersGBuffer")>, gBuffer);
    }

    void GLSLSurfelClusterAveraging::setSurfelsLuminaceMap(const GLHDRTexture2D& map) {
        setUniformTexture(uint32_constant<ctcrc32("uSurfelsLuminanceMap")>, map);
    }

}
