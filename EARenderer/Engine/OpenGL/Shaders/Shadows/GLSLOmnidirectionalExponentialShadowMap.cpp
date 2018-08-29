//
//  GLSLOmnidirectionalExponentialShadowMap.cpp
//  EARenderer
//
//  Created by Pavel Muratov on 8/29/18.
//  Copyright © 2018 MPO. All rights reserved.
//

#include "GLSLOmnidirectionalExponentialShadowMap.hpp"

#include <glm/gtc/type_ptr.hpp>

namespace EARenderer {

#pragma mark - Lifecycle

    GLSLOmnidirectionalExponentialShadowMap::GLSLOmnidirectionalExponentialShadowMap()
    :
    GLProgram("OmnidirectionalExponentialShadowMap.vert", "OmnidirectionalExponentialShadowMap.frag", "OmnidirectionalExponentialShadowMap.geom")
    { }

#pragma mark - Setters

    void GLSLOmnidirectionalExponentialShadowMap::setModelMatrix(const glm::mat4& matrix) {
        glUniformMatrix4fv(uniformByNameCRC32(ctcrc32("uModelMatrix")).location(), 1, GL_FALSE, glm::value_ptr(matrix));
    }

    void GLSLOmnidirectionalExponentialShadowMap::setLight(const PointLight& light) {
        glUniform3fv(uniformByNameCRC32(ctcrc32("uLight.position")).location(), 1, glm::value_ptr(light.position()));
        glUniform1f(uniformByNameCRC32(ctcrc32("uLight.farClipPlane")).location(), light.clipDistance());
        glUniformMatrix4fv(uniformByNameCRC32(ctcrc32("uLightSpaceMatrices[0]")).location(), 6, GL_FALSE, reinterpret_cast<const GLfloat *>(light.viewProjectionMatrices().data()));
    }

    void GLSLOmnidirectionalExponentialShadowMap::setESMFactor(float factor) {
        glUniform1f(uniformByNameCRC32(ctcrc32("uESMFactor")).location(), factor);
    }

}
