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

    void GLSLDirectionalExponentialShadowMap::setCamera(const Camera& camera) {
        glUniformMatrix4fv(uniformByNameCRC32(ctcrc32("uCameraMatrix")).location(), 1, GL_FALSE, glm::value_ptr(camera.viewProjectionMatrix()));
    }
    
    void GLSLDirectionalExponentialShadowMap::setESMFactor(float factor) {
        glUniform1f(uniformByNameCRC32(ctcrc32("uESMFactor")).location(), factor);
    }

    void GLSLDirectionalExponentialShadowMap::setFrustumCascades(const FrustumCascades& cascades) {
//        glUniform1i(uniformByNameCRC32(uint32_constant<ctcrc32("uNumberOfCascades")>).location(), cascades.amount);

        glUniformMatrix4fv(uniformByNameCRC32(uint32_constant<ctcrc32("uLightSpaceMatrices[0]")>).location(),
                           static_cast<GLsizei>(cascades.lightViewProjections.size()), GL_FALSE,
                           reinterpret_cast<const GLfloat *>(cascades.lightViewProjections.data()));

        glUniform1fv(uniformByNameCRC32(uint32_constant<ctcrc32("uDepthSplits[0]")>).location(),
                     static_cast<GLsizei>(cascades.splits.size()),
                     reinterpret_cast<const GLfloat *>(cascades.splits.data()));
    }

}
