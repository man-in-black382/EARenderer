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
        glUniformMatrix4fv(uniformByName("uCameraSpaceMat").location(), 1, GL_FALSE, glm::value_ptr(camera.viewProjectionMatrix()));
        glUniform3fv(uniformByName("uCameraPosition").location(), 1, glm::value_ptr(camera.position()));
    }
    
    void GLSLDirectionalBlinnPhong::setDirectionalLight(const DirectionalLight& light) {
        glUniform3fv(uniformByName("uLightDirection").location(), 1, glm::value_ptr(light.direction()));
        glUniform3fv(uniformByName("uLightColor").location(), 1, reinterpret_cast<const GLfloat *>(&light.color()));
    }
    
    void GLSLDirectionalBlinnPhong::setModelMatrix(const glm::mat4& matrix) {
        glUniformMatrix4fv(uniformByName("uModelMat").location(), 1, GL_FALSE, glm::value_ptr(matrix));
        glUniformMatrix4fv(uniformByName("uNormalMat").location(), 1, GL_FALSE, glm::value_ptr(glm::transpose(glm::inverse(matrix))));
    }
    
    void GLSLDirectionalBlinnPhong::setMaterial(const ClassicMaterial& material)
    {
        glUniform3fv(uniformByName("uMaterial.ambientReflectances").location(), 1, glm::value_ptr(material.ambientReflectances()));
        glUniform3fv(uniformByName("uMaterial.diffuseReflectances").location(), 1, glm::value_ptr(material.diffuseReflectances()));
        glUniform3fv(uniformByName("uMaterial.specularReflectances").location(), 1, glm::value_ptr(material.specularReflectances()));
        glUniform1f(uniformByName("uMaterial.specularExponent").location(), material.specularExponent());
        
        if (material.skin()) {
            setUniformTexture("uMaterial.diffuseMap", *material.skin());
        }
        
        if (material.normalMap()) {
            setUniformTexture("uMaterial.normalMap", *material.normalMap());
            glUniform1d(uniformByName("uIsNormalMappingEnabled").location(), GL_TRUE);
        }
    }
    
    void GLSLDirectionalBlinnPhong::setShadowCascades(const FrustumCascades& cascades) {
        uint8_t numberOfCascades = cascades.splits.size();
        glUniform1i(uniformByName("uNumberOfCascades").location(), numberOfCascades);
        glUniformMatrix4fv(uniformByName("uLightSpaceMatrices[0]").location(), static_cast<GLsizei>(cascades.lightViewProjections.size()), GL_FALSE, reinterpret_cast<const GLfloat *>(cascades.lightViewProjections.data()));
        glUniform1fv(uniformByName("uDepthSplits[0]").location(), numberOfCascades, reinterpret_cast<const GLfloat *>(cascades.splits.data()));
    }
    
    void GLSLDirectionalBlinnPhong::setShadowMaps(const GLDepthTexture2DArray& shadowMaps) {
        setUniformTexture("uShadowMaps", shadowMaps);
    }
    
    void GLSLDirectionalBlinnPhong::setHighlightColor(const Color& color) {
        glUniform4fv(uniformByName("uHighlightColor").location(), 1, reinterpret_cast<const GLfloat *>(&color));
    }
    
}
