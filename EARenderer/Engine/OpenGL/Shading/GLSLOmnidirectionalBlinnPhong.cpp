//
//  GLBlinnPhongProgram.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 11.04.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "GLSLOmnidirectionalBlinnPhong.hpp"

#include <glm/gtc/type_ptr.hpp>

namespace EARenderer {
    
#pragma mark - Lifecycle
    
    GLSLOmnidirectionalBlinnPhong::GLSLOmnidirectionalBlinnPhong()
    :
    GLProgram("OmnidirectionalBlinnPhong.vert", "OmnidirectionalBlinnPhong.frag", "")
    { }
    
#pragma mark - Setters
    
    void GLSLOmnidirectionalBlinnPhong::setCamera(const Camera& camera) {
        glUniformMatrix4fv(uniformLocation("uCamera.viewProjMat"), 1, GL_FALSE, glm::value_ptr(camera.viewProjectionMatrix()));
        glUniform3fv(uniformLocation("uCamera.position"), 1, glm::value_ptr(camera.position()));
    }
    
    void GLSLOmnidirectionalBlinnPhong::setPointLight(const PointLight& light) {
        glUniform3fv(uniformLocation("uLight.position"), 1, glm::value_ptr(light.position()));
        glUniform3fv(uniformLocation("uLight.color"), 1, reinterpret_cast<const GLfloat *>(&light.color()));
        glUniform1f(uniformLocation("uLight.farClipPlane"), light.clipDistance());
    }
    
    void GLSLOmnidirectionalBlinnPhong::setModelMatrix(const glm::mat4& matrix) {
        glUniformMatrix4fv(uniformLocation("uModelMat"), 1, GL_FALSE, glm::value_ptr(matrix));
        glUniformMatrix4fv(uniformLocation("uNormalMat"), 1, GL_FALSE, glm::value_ptr(glm::transpose(glm::inverse(matrix))));
    }
    
    void GLSLOmnidirectionalBlinnPhong::setMaterial(const Material& material)
    {
        glUniform3fv(uniformLocation("uMaterial.ambientReflectances"), 1, glm::value_ptr(material.ambientReflectances()));
        glUniform3fv(uniformLocation("uMaterial.diffuseReflectances"), 1, glm::value_ptr(material.diffuseReflectances()));
        glUniform3fv(uniformLocation("uMaterial.specularReflectances"), 1, glm::value_ptr(material.specularReflectances()));
        glUniform1f(uniformLocation("uMaterial.specularExponent"), material.specularExponent());
        
        setUniformTexture("uMaterial.diffuseTexture", material.skin());
    }
    
    void GLSLOmnidirectionalBlinnPhong::setShadowMap(const GLDepthTextureCubemap& shadowMap) {
        setUniformTexture("uShadowCubeMap", shadowMap);
    }
    
    void GLSLOmnidirectionalBlinnPhong::setHighlightColor(const Color& color) {
        glUniform4fv(uniformLocation("uHighlightColor"), 1, reinterpret_cast<const GLfloat *>(&color));
    }
    
}
