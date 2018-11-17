//
//  GLSLDirectionalShadowMap.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 13.11.2018.
//  Copyright © 2018 MPO. All rights reserved.
//

#include "GLSLDirectionalShadowMap.hpp"

#include <glm/gtc/type_ptr.hpp>

namespace EARenderer {
    
#pragma mark - Lifecycle
    
    GLSLDirectionalShadowMap::GLSLDirectionalShadowMap()
    :
    GLProgram("DirectionalShadowMap.vert", "DirectionalShadowMap.frag", "DirectionalShadowMap.geom")
    { }
    
#pragma mark - Setters
    
    void GLSLDirectionalShadowMap::setModelMatrix(const glm::mat4& modelMatrix) {
        glUniformMatrix4fv(uniformByNameCRC32(ctcrc32("uModelMatrix")).location(), 1, GL_FALSE, glm::value_ptr(modelMatrix));
    }

    void GLSLDirectionalShadowMap::setCascades(const FrustumCascades& cascades) {
        glUniformMatrix4fv(uniformByNameCRC32(ctcrc32("uLightSpaceMatrices[0]")).location(),
                           (GLsizei)cascades.lightViewProjections.size(),
                           GL_FALSE,
                           (GLfloat *)cascades.lightViewProjections.data());
    }
    
}
