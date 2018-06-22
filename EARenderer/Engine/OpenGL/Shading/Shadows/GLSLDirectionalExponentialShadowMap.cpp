//
//  GLSLDirectionalShadowMap.cpp
//  EARenderer
//
//  Created by Pavel Muratov on 6/13/18.
//  Copyright © 2018 MPO. All rights reserved.
//

#include "GLSLDirectionalExponentialShadowMap.hpp"

#include <glm/gtc/type_ptr.hpp>

namespace EARenderer {

#pragma mark - Lifecycle

    GLSLDirectionalExponentialShadowMap::GLSLDirectionalExponentialShadowMap()
    :
    GLProgram("DirectionalExponentialShadowMap.vert", "DirectionalExponentialShadowMap.frag", "")
    { }

#pragma mark - Setters

    void GLSLDirectionalExponentialShadowMap::setModelMatrix(const glm::mat4& modelMatrix) {
        glUniformMatrix4fv(uniformByNameCRC32(ctcrc32("uModelMatrix")).location(), 1, GL_FALSE, glm::value_ptr(modelMatrix));
    }

    void GLSLDirectionalExponentialShadowMap::setLightMatrix(const glm::mat4& lightMatrix) {
        glUniformMatrix4fv(uniformByNameCRC32(ctcrc32("uLightSpaceMatrix")).location(), 1, GL_FALSE, glm::value_ptr(lightMatrix));
    }
    
    void GLSLDirectionalExponentialShadowMap::setESMFactor(float factor) {
        glUniform1f(uniformByNameCRC32(ctcrc32("uESMFactor")).location(), factor);
    }

}
