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
        constexpr auto cameraSpace = ctcrc32("uCameraSpaceMat");
        constexpr auto cameraPosition = ctcrc32("uCameraPosition");
        
        glUniformMatrix4fv(uniformByNameCRC32(cameraSpace).location(), 1, GL_FALSE, glm::value_ptr(camera.viewProjectionMatrix()));
        glUniform3fv(uniformByNameCRC32(cameraPosition).location(), 1, glm::value_ptr(camera.position()));
    }
    
    void GLSLCookTorrance::setModelMatrix(const glm::mat4& matrix) {
        constexpr auto modelMat = ctcrc32("uModelMat");
        constexpr auto normalMat = ctcrc32("uNormalMat");
        
        glUniformMatrix4fv(uniformByNameCRC32(modelMat).location(), 1, GL_FALSE, glm::value_ptr(matrix));
        glUniformMatrix4fv(uniformByNameCRC32(normalMat).location(), 1, GL_FALSE, glm::value_ptr(glm::transpose(glm::inverse(matrix))));
    }
    
    void GLSLCookTorrance::setLight(const PointLight& light) {
        constexpr auto lightPos = ctcrc32("uPointLight.position");
        constexpr auto lightFlux = ctcrc32("uPointLight.radiantFlux");
        
        glUniform3fv(uniformByNameCRC32(lightPos).location(), 1, glm::value_ptr(light.position()));
        glUniform3fv(uniformByNameCRC32(lightFlux).location(), 1, reinterpret_cast<const GLfloat *>(&light.color()));
    }
    
    void GLSLCookTorrance::setMaterial(const PBRMaterial& material) {
        constexpr auto albedo = ctcrc32("uMaterial.albedoMap");
        constexpr auto normal = ctcrc32("uMaterial.normalMap");
        constexpr auto metallic = ctcrc32("uMaterial.metallicMap");
        constexpr auto roughness = ctcrc32("uMaterial.roughnessMap");
        constexpr auto ao = ctcrc32("uMaterial.AOMap");
        
        if (material.albedoMap()) { setUniformTexture(albedo, *material.albedoMap()); }
        if (material.normalMap()) { setUniformTexture(normal, *material.normalMap()); }
        if (material.metallicMap()) { setUniformTexture(metallic, *material.metallicMap()); }
        if (material.roughnessMap()) { setUniformTexture(roughness, *material.roughnessMap()); }
        if (material.ambientOcclusionMap()) { setUniformTexture(ao, *material.ambientOcclusionMap()); }
    }
    
    void GLSLCookTorrance::setIBLUniforms(const GLHDRTextureCubemap& diffuseIrradianceMap,
                                          const GLHDRTextureCubemap& specularIrradianceMap,
                                          const GLHDRTexture2D& BRDFIntegrationMap,
                                          int8_t specularIrradianceMapMaxLOD)
    {
        constexpr auto diffuse = ctcrc32("uDiffuseIrradianceMap");
        constexpr auto specular = ctcrc32("uSpecularIrradianceMap");
        constexpr auto brdf = ctcrc32("uBRDFIntegrationMap");
        constexpr auto lod = ctcrc32("uSpecularIrradianceMapLOD");
        
        setUniformTexture(diffuse, diffuseIrradianceMap);
        setUniformTexture(specular, specularIrradianceMap);
        setUniformTexture(brdf, BRDFIntegrationMap);
        glUniform1i(uniformByNameCRC32(lod).location(), specularIrradianceMapMaxLOD);
    }
    
}
