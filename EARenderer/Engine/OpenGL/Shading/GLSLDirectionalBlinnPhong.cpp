//
//  GLBlinnPhongProgram.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 11.04.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "GLSLDirectionalBlinnPhong.hpp"

#include <glm/gtc/type_ptr.hpp>

namespace EARenderer {
    
#pragma mark - Lifecycle
    
    GLSLDirectionalBlinnPhong::GLSLDirectionalBlinnPhong()
    :
    GLProgram("DirectionalBlinnPhong.vert", "DirectionalBlinnPhong.frag", "")
    { }
    
#pragma mark - Setters
    
    void GLSLDirectionalBlinnPhong::setCamera(const Camera& camera) {
        glUniformMatrix4fv(uniformLocation("uCameraSpaceMat"), 1, GL_FALSE, glm::value_ptr(camera.viewProjectionMatrix()));
        glUniform3fv(uniformLocation("uCameraPosition"), 1, glm::value_ptr(camera.position()));
    }
    
    void GLSLDirectionalBlinnPhong::setDirectionalLight(const DirectionalLight& light) {
        glUniform3fv(uniformLocation("uLightDirection"), 1, glm::value_ptr(light.direction()));
        glUniformMatrix4fv(uniformLocation("uLightSpaceMat"), 1, GL_FALSE, glm::value_ptr(light.viewProjectionMatrix()));
        glUniform3fv(uniformLocation("uLightColor"), 1, reinterpret_cast<const GLfloat *>(&light.color()));
    }
    
    void GLSLDirectionalBlinnPhong::setModelMatrix(const glm::mat4& matrix) {
        glUniformMatrix4fv(uniformLocation("uModelMat"), 1, GL_FALSE, glm::value_ptr(matrix));
        glUniformMatrix4fv(uniformLocation("uNormalMat"), 1, GL_FALSE, glm::value_ptr(glm::transpose(glm::inverse(matrix))));
    }
    
    void GLSLDirectionalBlinnPhong::setMaterial(const Material& material)
    {
        glUniform3fv(uniformLocation("uMaterial.ambientReflectances"), 1, glm::value_ptr(material.ambientReflectances()));
        glUniform3fv(uniformLocation("uMaterial.diffuseReflectances"), 1, glm::value_ptr(material.diffuseReflectances()));
        glUniform3fv(uniformLocation("uMaterial.specularReflectances"), 1, glm::value_ptr(material.specularReflectances()));
        glUniform1f(uniformLocation("uMaterial.specularExponent"), material.specularExponent());
        
        setUniformTexture("uMaterial.diffuseTexture", material.skin());
    }
    
    void GLSLDirectionalBlinnPhong::setShadowCascades(const FrustumCascades& cascades) {
        uint8_t numberOfCascades = cascades.splits.size();
        glUniform1i(uniformLocation("uNumberOfCascades"), numberOfCascades);
        glUniform1fv(uniformLocation("uDepthSplits"), numberOfCascades, cascades.splits.data());
    }
    
    void GLSLDirectionalBlinnPhong::setShadowMaps(const GLDepthTexture2DArray& shadowMaps) {
        setUniformTexture("uShadowMaps", shadowMaps);
    }
    
    void GLSLDirectionalBlinnPhong::setHighlightColor(const Color& color) {
        glUniform4fv(uniformLocation("uHighlightColor"), 1, reinterpret_cast<const GLfloat *>(&color));
    }
    
}
