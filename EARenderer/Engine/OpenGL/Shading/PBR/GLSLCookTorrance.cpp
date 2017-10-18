//
//  GLSLCookTorrance.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 07.10.2017.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "GLSLCookTorrance.hpp"

#include <glm/gtc/type_ptr.hpp>

namespace EARenderer {
    
#pragma mark - Lifecycle
    
    GLSLCookTorrance::GLSLCookTorrance()
    :
    GLProgram("CookTorrance.vert", "CookTorrance.frag", "")
    { }
    
#pragma mark - Setters
    
    void GLSLCookTorrance::setCamera(const Camera& camera) {
        glUniformMatrix4fv(uniformByName("uCameraSpaceMat").location(), 1, GL_FALSE, glm::value_ptr(camera.viewProjectionMatrix()));
        glUniform3fv(uniformByName("uCameraPosition").location(), 1, glm::value_ptr(camera.position()));
    }
    
    void GLSLCookTorrance::setModelMatrix(const glm::mat4& matrix) {
        glUniformMatrix4fv(uniformByName("uModelMat").location(), 1, GL_FALSE, glm::value_ptr(matrix));
        glUniformMatrix4fv(uniformByName("uNormalMat").location(), 1, GL_FALSE, glm::value_ptr(glm::transpose(glm::inverse(matrix))));
    }
    
    void GLSLCookTorrance::setLight(const PointLight& light) {
        glUniform3fv(uniformByName("uPointLight.position").location(), 1, glm::value_ptr(light.position()));
        glUniform3fv(uniformByName("uPointLight.radiantFlux").location(), 1, reinterpret_cast<const GLfloat *>(&light.color()));
    }
    
    void GLSLCookTorrance::setMaterial(const PBRMaterial& material) {
        if (material.albedoMap()) { setUniformTexture("uMaterial.albedoMap", *material.albedoMap()); }
        if (material.normalMap()) { setUniformTexture("uMaterial.normalMap", *material.normalMap()); }
        if (material.metallicMap()) { setUniformTexture("uMaterial.metallicMap", *material.metallicMap()); }
        if (material.roughnessMap()) { setUniformTexture("uMaterial.roughnessMap", *material.roughnessMap()); }
        if (material.ambientOcclusionMap()) { setUniformTexture("uMaterial.AOMap", *material.ambientOcclusionMap()); }
    }
    
    void GLSLCookTorrance::setIBLUniforms(const GLHDRTextureCubemap& diffuseIrradianceMap,
                                          const GLHDRTextureCubemap& specularIrradianceMap,
                                          const GLHDRTexture2D& BRDFIntegrationMap,
                                          int8_t specularIrradianceMapMaxLOD)
    {
        setUniformTexture("uDiffuseIrradianceMap", diffuseIrradianceMap);
        setUniformTexture("uSpecularIrradianceMap", specularIrradianceMap);
        setUniformTexture("uBRDFIntegrationMap", BRDFIntegrationMap);
        glUniform1i(uniformByName("uSpecularIrradianceMapLOD").location(), specularIrradianceMapMaxLOD);
    }
    
}
