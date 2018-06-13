//
//  GLSLDirectionalShadowMap.cpp
//  EARenderer
//
//  Created by Pavel Muratov on 6/13/18.
//  Copyright © 2018 MPO. All rights reserved.
//

#include "GLSLDirectionalShadowMap.hpp"

#include <glm/gtc/type_ptr.hpp>

namespace EARenderer {

#pragma mark - Lifecycle

    GLSLDirectionalShadowMap::GLSLDirectionalShadowMap()
    :
    GLProgram("DirectionalShadowMap.vert", "DirectionalShadowMap.frag", "")
    { }

#pragma mark - Setters

    void GLSLDirectionalShadowMap::setViewProjectionMatrix(const glm::mat4& matrix) {
        glUniformMatrix4fv(uniformByNameCRC32(ctcrc32("uViewProjectionMatrix")).location(), 1, GL_FALSE, glm::value_ptr(matrix));
    }

    void GLSLDirectionalShadowMap::setModelMatrix(const glm::mat4& matrix) {
        glUniformMatrix4fv(uniformByNameCRC32(ctcrc32("uModelMatrix")).location(), 1, GL_FALSE, glm::value_ptr(matrix));
    }

}
