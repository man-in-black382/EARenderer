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
        glUniformMatrix4fv(uniformByNameCRC32(uint32_constant<ctcrc32("uCameraSpaceMat")>).location(), 1, GL_FALSE, glm::value_ptr(camera.viewProjectionMatrix()));
        glUniform3fv(uniformByNameCRC32(uint32_constant<ctcrc32("uCameraPosition")>).location(), 1, glm::value_ptr(camera.position()));
    }
    
    void GLSLCookTorrance::setModelMatrix(const glm::mat4& matrix) {
        glUniformMatrix4fv(uniformByNameCRC32(uint32_constant<ctcrc32("uModelMat")>).location(), 1, GL_FALSE, glm::value_ptr(matrix));
        glUniformMatrix4fv(uniformByNameCRC32(uint32_constant<ctcrc32("uNormalMat")>).location(), 1, GL_FALSE, glm::value_ptr(glm::transpose(glm::inverse(matrix))));
    }
    
    void GLSLCookTorrance::setLight(const PointLight& light) {
        glUniform3fv(uniformByNameCRC32(uint32_constant<ctcrc32("uPointLight.position")>).location(), 1, glm::value_ptr(light.position()));
        glUniform3fv(uniformByNameCRC32(uint32_constant<ctcrc32("uPointLight.radiantFlux")>).location(), 1, reinterpret_cast<const GLfloat *>(&light.color()));
    }
    
    void GLSLCookTorrance::setMaterial(const PBRMaterial& material) {
        if (material.albedoMap()) { setUniformTexture(uint32_constant<ctcrc32("uMaterial.albedoMap")>, *material.albedoMap()); }
        if (material.normalMap()) { setUniformTexture(uint32_constant<ctcrc32("uMaterial.normalMap")>, *material.normalMap()); }
        if (material.metallicMap()) { setUniformTexture(uint32_constant<ctcrc32("uMaterial.metallicMap")>, *material.metallicMap()); }
        if (material.roughnessMap()) { setUniformTexture(uint32_constant<ctcrc32("uMaterial.roughnessMap")>, *material.roughnessMap()); }
        if (material.ambientOcclusionMap()) { setUniformTexture(uint32_constant<ctcrc32("uMaterial.AOMap")>, *material.ambientOcclusionMap()); }
    }
    
    void GLSLCookTorrance::setIBLUniforms(const GLHDRTextureCubemap& diffuseIrradianceMap,
                                          const GLHDRTextureCubemap& specularIrradianceMap,
                                          const GLHDRTexture2D& BRDFIntegrationMap,
                                          int8_t specularIrradianceMapMaxLOD)
    {
        setUniformTexture(uint32_constant<ctcrc32("uDiffuseIrradianceMap")>, diffuseIrradianceMap);
        setUniformTexture(uint32_constant<ctcrc32("uSpecularIrradianceMap")>, specularIrradianceMap);
        setUniformTexture(uint32_constant<ctcrc32("uBRDFIntegrationMap")>, BRDFIntegrationMap);
        glUniform1i(uniformByNameCRC32(uint32_constant<ctcrc32("uSpecularIrradianceMapLOD")>).location(), specularIrradianceMapMaxLOD);
    }
    
}
