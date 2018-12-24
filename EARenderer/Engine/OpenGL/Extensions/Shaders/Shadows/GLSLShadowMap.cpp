//
//  GLSLDirectionalShadowMap.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 13.11.2018.
//  Copyright © 2018 MPO. All rights reserved.
//

#include "GLSLShadowMap.hpp"

#include <glm/gtc/type_ptr.hpp>

namespace EARenderer {

#pragma mark - Lifecycle

    GLSLShadowMap::GLSLShadowMap()
            :
            GLProgram("ShadowMap.vert", "ShadowMap.frag", "ShadowMap.geom") {
    }

#pragma mark - Setters

    void GLSLShadowMap::setModelMatrix(const glm::mat4 &modelMatrix) {
        glUniformMatrix4fv(uniformByNameCRC32(ctcrc32("uModelMatrix")).location(), 1, GL_FALSE, glm::value_ptr(modelMatrix));
    }

    void GLSLShadowMap::setViewProjectionMatrices(const std::vector<glm::mat4> &matrices) {
        glUniformMatrix4fv(uniformByNameCRC32(ctcrc32("uLightSpaceMatrices[0]")).location(),
                (GLsizei) matrices.size(),
                GL_FALSE,
                (GLfloat *) matrices.data());
    }

}
