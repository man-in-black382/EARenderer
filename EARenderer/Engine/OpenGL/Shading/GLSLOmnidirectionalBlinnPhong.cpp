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
        glUniformMatrix4fv(uniformByName("uCamera.viewProjMat").location(), 1, GL_FALSE, glm::value_ptr(camera.viewProjectionMatrix()));
        glUniform3fv(uniformByName("uCamera.position").location(), 1, glm::value_ptr(camera.position()));
    }
    
    void GLSLOmnidirectionalBlinnPhong::setPointLight(const PointLight& light) {
        glUniform3fv(uniformByName("uLight.position").location(), 1, glm::value_ptr(light.position()));
        glUniform3fv(uniformByName("uLight.color").location(), 1, reinterpret_cast<const GLfloat *>(&light.color()));
        glUniform1f(uniformByName("uLight.farClipPlane").location(), light.clipDistance());
    }
    
    void GLSLOmnidirectionalBlinnPhong::setModelMatrix(const glm::mat4& matrix) {
        glUniformMatrix4fv(uniformByName("uModelMat").location(), 1, GL_FALSE, glm::value_ptr(matrix));
        glUniformMatrix4fv(uniformByName("uNormalMat").location(), 1, GL_FALSE, glm::value_ptr(glm::transpose(glm::inverse(matrix))));
    }
    
    void GLSLOmnidirectionalBlinnPhong::setMaterial(const ClassicMaterial& material)
    {
        glUniform3fv(uniformByName("uMaterial.ambientReflectances").location(), 1, glm::value_ptr(material.ambientReflectances()));
        glUniform3fv(uniformByName("uMaterial.diffuseReflectances").location(), 1, glm::value_ptr(material.diffuseReflectances()));
        glUniform3fv(uniformByName("uMaterial.specularReflectances").location(), 1, glm::value_ptr(material.specularReflectances()));
        glUniform1f(uniformByName("uMaterial.specularExponent").location(), material.specularExponent());
        
        if (material.skin()) {
            setUniformTexture("uMaterial.diffuseTexture", *material.skin());
        }
    }
    
    void GLSLOmnidirectionalBlinnPhong::setShadowMap(const GLDepthTextureCubemap& shadowMap) {
        setUniformTexture("uShadowCubeMap", shadowMap);
    }
    
    void GLSLOmnidirectionalBlinnPhong::setHighlightColor(const Color& color) {
        glUniform4fv(uniformByName("uHighlightColor").location(), 1, reinterpret_cast<const GLfloat *>(&color));
    }
    
}
