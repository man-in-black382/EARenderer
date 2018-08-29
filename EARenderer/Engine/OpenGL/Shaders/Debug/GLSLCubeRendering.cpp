//
//  GLSLCubeRendering.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 08.03.2018.
//  Copyright © 2018 MPO. All rights reserved.
//

#include "GLSLCubeRendering.hpp"

namespace EARenderer {

#pragma mark - Lifecycle

    GLSLCubeRendering::GLSLCubeRendering(Mode mode)
    :
    GLProgram("CubeRendering.vert",
              "GenericGeometry.frag",
              mode == Mode::Sides ? "CubeSidesRendering.geom" : "CubeEdgesRendering.geom")
    { }

#pragma mark - Setters

    void GLSLCubeRendering::setViewProjectionMatrix(const glm::mat4 &mvp) {
        glUniformMatrix4fv(uniformByNameCRC32(ctcrc32("uViewProjectionMatrix")).location(), 1, GL_FALSE, glm::value_ptr(mvp));
    }

    void GLSLCubeRendering::setColor(const Color& color) {
        glUniform4fv(uniformByNameCRC32(ctcrc32("uColor")).location(), 1, reinterpret_cast<const float *>(&color));
    }

}
