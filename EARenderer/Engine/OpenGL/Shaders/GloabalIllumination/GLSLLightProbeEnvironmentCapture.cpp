//
//  GLSLLightProbeEnvironmentCapture.cpp
//  EARenderer
//
//  Created by Pavel Muratov on 12/4/17.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "GLSLLightProbeEnvironmentCapture.hpp"

namespace EARenderer {
    
#pragma mark - Lifecycle
    
    GLSLLightProbeEnvironmentCapture::GLSLLightProbeEnvironmentCapture()
    :
    EARenderer::GLProgram("LightProbeEnvironmentCapture.vert", "CookTorrance.frag", "LightProbeEnvironmentCapture.geom")
    {
//        glUniform1i(uniformByNameCRC32(uint32_constant<ctcrc32("uShouldEvaluateSphericalHarmonics")>).location(), GL_FALSE);
    }
    
#pragma mark - Setters
    
    void GLSLLightProbeEnvironmentCapture::setModelMatrix(const glm::mat4 &modelMatrix) {
        glUniformMatrix4fv(uniformByNameCRC32(uint32_constant<ctcrc32("uModelMat")>).location(), 1, GL_FALSE, glm::value_ptr(modelMatrix));
        glUniformMatrix4fv(uniformByNameCRC32(uint32_constant<ctcrc32("uNormalMat")>).location(), 1, GL_FALSE, glm::value_ptr(glm::transpose(glm::inverse(modelMatrix))));
    }
    
    void GLSLLightProbeEnvironmentCapture::setLightProbe(const LightProbe& probe) {
        glUniform3fv(uniformByNameCRC32(uint32_constant<ctcrc32("uCameraPosition")>).location(), 1, glm::value_ptr(probe.position));
        glUniformMatrix4fv(uniformByNameCRC32(uint32_constant<ctcrc32("uViewProjectionMatrices[0]")>).location(), 6, GL_FALSE, reinterpret_cast<const GLfloat *>(probe.viewProjectionMatrices().data()));
    }
    
    void GLSLLightProbeEnvironmentCapture::setLight(const PointLight& light) {
        glUniform3fv(uniformByNameCRC32(uint32_constant<ctcrc32("uPointLight.position")>).location(), 1, glm::value_ptr(light.position()));
        glUniform3fv(uniformByNameCRC32(uint32_constant<ctcrc32("uPointLight.radiantFlux")>).location(), 1, reinterpret_cast<const GLfloat *>(&light.color()));
        glUniform1i(uniformByNameCRC32(uint32_constant<ctcrc32("uLightType")>).location(), 1);
    }
    
    void GLSLLightProbeEnvironmentCapture::setLight(const DirectionalLight& light) {
        glUniform3fv(uniformByNameCRC32(uint32_constant<ctcrc32("uDirectionalLight.direction")>).location(), 1, glm::value_ptr(light.direction()));
        glUniform3fv(uniformByNameCRC32(uint32_constant<ctcrc32("uDirectionalLight.radiantFlux")>).location(), 1, reinterpret_cast<const GLfloat *>(&light.color()));
        glUniform1i(uniformByNameCRC32(uint32_constant<ctcrc32("uLightType")>).location(), 0);
    }
    
    void GLSLLightProbeEnvironmentCapture::setMaterial(const PBRMaterial& material) {
        if (material.albedoMap()) { setUniformTexture(uint32_constant<ctcrc32("uMaterial.albedoMap")>, *material.albedoMap()); }
        if (material.normalMap()) { setUniformTexture(uint32_constant<ctcrc32("uMaterial.normalMap")>, *material.normalMap()); }
        if (material.metallicMap()) { setUniformTexture(uint32_constant<ctcrc32("uMaterial.metallicMap")>, *material.metallicMap()); }
        if (material.roughnessMap()) { setUniformTexture(uint32_constant<ctcrc32("uMaterial.roughnessMap")>, *material.roughnessMap()); }
        if (material.ambientOcclusionMap()) { setUniformTexture(uint32_constant<ctcrc32("uMaterial.AOMap")>, *material.ambientOcclusionMap()); }
    }

}
