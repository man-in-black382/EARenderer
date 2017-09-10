//
//  GLSLOmnidirectionalDepth.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 26.07.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "GLSLOmnidirectionalDepth.hpp"

#include <glm/gtc/type_ptr.hpp>

namespace EARenderer {
    
#pragma mark - Lifecycle
    
    GLSLOmnidirectionalDepth::GLSLOmnidirectionalDepth()
    :
    GLProgram("OmnidirectionalDepth.vert", "OmnidirectionalDepth.frag", "OmnidirectionalDepth.geom")
    { }
    
#pragma mark - Setters
    
    void GLSLOmnidirectionalDepth::setModelMatrix(const glm::mat4& matrix) {
        glUniformMatrix4fv(uniformByName("uModelMatrix").location(), 1, GL_FALSE, glm::value_ptr(matrix));
    }
    
    void GLSLOmnidirectionalDepth::setLight(const PointLight& light) {
        glUniform3fv(uniformByName("uLight.position").location(), 1, glm::value_ptr(light.position()));
        glUniform1f(uniformByName("uLight.farClipPlane").location(), light.clipDistance());
        glUniformMatrix4fv(uniformByName("uLightSpaceMatrices[0]").location(), 6, GL_FALSE, reinterpret_cast<const GLfloat *>(light.viewProjectionMatrices().data()));
    }
    
}

