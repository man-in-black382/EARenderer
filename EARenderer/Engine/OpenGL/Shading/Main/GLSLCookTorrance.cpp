//
//  GLSLCookTorrance.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 07.10.2017.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "GLSLCookTorrance.hpp"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>

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
        glUniform1i(uniformByNameCRC32(uint32_constant<ctcrc32("uLightType")>).location(), 1);
    }
    
    void GLSLCookTorrance::setLight(const DirectionalLight& light) {
        glUniform3fv(uniformByNameCRC32(uint32_constant<ctcrc32("uDirectionalLight.direction")>).location(), 1, glm::value_ptr(light.direction()));
        glUniform3fv(uniformByNameCRC32(uint32_constant<ctcrc32("uDirectionalLight.radiantFlux")>).location(), 1, reinterpret_cast<const GLfloat *>(&light.color()));
        glUniform1i(uniformByNameCRC32(uint32_constant<ctcrc32("uLightType")>).location(), 0);
    }
    
    void GLSLCookTorrance::setMaterial(const PBRMaterial& material) {
        if (material.albedoMap()) { setUniformTexture(uint32_constant<ctcrc32("uMaterial.albedoMap")>, *material.albedoMap()); }
        if (material.normalMap()) { setUniformTexture(uint32_constant<ctcrc32("uMaterial.normalMap")>, *material.normalMap()); }
        if (material.metallicMap()) { setUniformTexture(uint32_constant<ctcrc32("uMaterial.metallicMap")>, *material.metallicMap()); }
        if (material.roughnessMap()) { setUniformTexture(uint32_constant<ctcrc32("uMaterial.roughnessMap")>, *material.roughnessMap()); }
        if (material.ambientOcclusionMap()) { setUniformTexture(uint32_constant<ctcrc32("uMaterial.AOMap")>, *material.ambientOcclusionMap()); }
        if (material.displacementMap()) { setUniformTexture(uint32_constant<ctcrc32("uMaterial.displacementMap")>, *material.displacementMap()); }
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
    
    void GLSLCookTorrance::setFrustumCascades(const FrustumCascades& cascades) {
//        glUniformMatrix4fv(uniformByNameCRC32(uint32_constant<ctcrc32("uLightSpaceMat")>).location(), 1, GL_FALSE,
//                           reinterpret_cast<const GLfloat *>(cascades.lightViewProjections.data()));

//        glUniform1i(uniformByNameCRC32(uint32_constant<ctcrc32("uNumberOfCascades")>).location(), cascades.amount);

        glUniformMatrix4fv(uniformByNameCRC32(uint32_constant<ctcrc32("uLightSpaceMatrices[0]")>).location(),
                           static_cast<GLsizei>(cascades.lightViewProjections.size()), GL_FALSE,
                           reinterpret_cast<const GLfloat *>(cascades.lightViewProjections.data()));

        glUniform1fv(uniformByNameCRC32(uint32_constant<ctcrc32("uDepthSplits[0]")>).location(),
                     static_cast<GLsizei>(cascades.splits.size()),
                     reinterpret_cast<const GLfloat *>(cascades.splits.data()));

        glUniformMatrix4fv(uniformByNameCRC32(uint32_constant<ctcrc32("uCSMSplitSpaceMat")>).location(), 1, GL_FALSE, glm::value_ptr(cascades.splitSpaceMatrix));
    }
    
//    void GLSLCookTorrance::setExponentialShadowMap(const GLHDRTexture2D& map) {
//        setUniformTexture(uint32_constant<ctcrc32("uExponentialShadowMap")>, map);
//    }
    
    void GLSLCookTorrance::setGridProbesSHTextures(const std::array<GLLDRTexture3D, 4>& textures) {
        setUniformTexture(uint32_constant<ctcrc32("uGridSHMap0")>, textures[0]);
        setUniformTexture(uint32_constant<ctcrc32("uGridSHMap1")>, textures[1]);
//        setUniformTexture(uint32_constant<ctcrc32("uGridSHMap2")>, textures[2]);
//        setUniformTexture(uint32_constant<ctcrc32("uGridSHMap3")>, textures[3]);
    }
    
    void GLSLCookTorrance::setWorldBoundingBox(const AxisAlignedBox3D& box) {
        glUniformMatrix4fv(uniformByNameCRC32(uint32_constant<ctcrc32("uWorldBoudningBoxTransform")>).location(), 1, GL_FALSE, glm::value_ptr(box.localSpaceMatrix()));
    }
    
    void GLSLCookTorrance::setProbePositions(const GLFloat3BufferTexture<glm::vec3>& positions) {
        setUniformTexture(uint32_constant<ctcrc32("uProbePositions")>, positions);
    }

    void GLSLCookTorrance::setSettings(const RenderingSettings& settings) {
        glUniform1ui(uniformByNameCRC32(uint32_constant<ctcrc32("uSettingsBitmask")>).location(), settings.meshSettings.booleanBitmask());
//        glUniform1f(uniformByNameCRC32(uint32_constant<ctcrc32("uParallaxMappingStrength")>).location(), settings.meshSettings.parallaxMappingStrength);
        glUniform1f(uniformByNameCRC32(uint32_constant<ctcrc32("uESMFactor")>).location(), settings.meshSettings.ESMFactor);

//        int32_t compression = 0;
//        switch (settings.meshSettings.SHCompression) {
//            case RenderingSettings::MeshSettings::SphericalHarmonicsCompression::Uncompressed: compression = 0; break;
//            case RenderingSettings::MeshSettings::SphericalHarmonicsCompression::Compressed322: compression = 1; break;
//            case RenderingSettings::MeshSettings::SphericalHarmonicsCompression::Compressed311: compression = 2; break;
//        }
//        glUniform1i(uniformByNameCRC32(uint32_constant<ctcrc32("uSHCompressionType")>).location(), compression);
    }
    
}
