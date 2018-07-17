//
//  GLSLDeferredCookTorrance.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 30.06.2018.
//  Copyright © 2018 MPO. All rights reserved.
//

#include "GLSLDeferredCookTorrance.hpp"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>

namespace EARenderer {

#pragma mark - Lifecycle

    GLSLDeferredCookTorrance::GLSLDeferredCookTorrance()
    :
    GLProgram("FullScreenQuad.vert", "DeferredCookTorrance.frag", "")
    { }

#pragma mark - Setters

    void GLSLDeferredCookTorrance::setCamera(const Camera& camera) {
        glUniform3fv(uniformByNameCRC32(uint32_constant<ctcrc32("uCameraPosition")>).location(), 1, glm::value_ptr(camera.position()));
        glUniformMatrix4fv(uniformByNameCRC32(uint32_constant<ctcrc32("uCameraViewInverse")>).location(), 1, GL_FALSE,
                           glm::value_ptr(camera.inverseViewMatrix()));
        glUniformMatrix4fv(uniformByNameCRC32(uint32_constant<ctcrc32("uCameraProjectionInverse")>).location(), 1, GL_FALSE,
                           glm::value_ptr(camera.inverseProjectionMatrix()));

        glm::vec2 nearFar(camera.nearClipPlane(), camera.farClipPlane());
        glUniform2fv(uniformByNameCRC32(uint32_constant<ctcrc32("uCameraNearFarPlanes")>).location(), 1, glm::value_ptr(nearFar));
    }

    void GLSLDeferredCookTorrance::setLight(const PointLight& light) {
        glUniform3fv(uniformByNameCRC32(uint32_constant<ctcrc32("uPointLight.position")>).location(), 1, glm::value_ptr(light.position()));
        glUniform3fv(uniformByNameCRC32(uint32_constant<ctcrc32("uPointLight.radiantFlux")>).location(), 1, reinterpret_cast<const GLfloat *>(&light.color()));
        glUniform1i(uniformByNameCRC32(uint32_constant<ctcrc32("uLightType")>).location(), 1);
    }

    void GLSLDeferredCookTorrance::setLight(const DirectionalLight& light) {
        glUniform3fv(uniformByNameCRC32(uint32_constant<ctcrc32("uDirectionalLight.direction")>).location(), 1, glm::value_ptr(light.direction()));
        glUniform3fv(uniformByNameCRC32(uint32_constant<ctcrc32("uDirectionalLight.radiantFlux")>).location(), 1, reinterpret_cast<const GLfloat *>(&light.color()));
        glUniform1i(uniformByNameCRC32(uint32_constant<ctcrc32("uLightType")>).location(), 0);
    }

    void GLSLDeferredCookTorrance::setGBuffer(const SceneGBuffer& GBuffer) {
        setUniformTexture(uint32_constant<ctcrc32("uGBufferAlbedoRoughnessMetalnessAONormal")>, GBuffer.albedoRoughnessMetalnessAONormal);
        setUniformTexture(uint32_constant<ctcrc32("uGBufferLinearDepth")>, GBuffer.linearDepthHZB);
    }

    void GLSLDeferredCookTorrance::setFrustumCascades(const FrustumCascades& cascades) {
        glUniformMatrix4fv(uniformByNameCRC32(uint32_constant<ctcrc32("uLightSpaceMatrices[0]")>).location(),
                           static_cast<GLsizei>(cascades.lightViewProjections.size()), GL_FALSE,
                           reinterpret_cast<const GLfloat *>(cascades.lightViewProjections.data()));

        glUniform1fv(uniformByNameCRC32(uint32_constant<ctcrc32("uDepthSplits[0]")>).location(),
                     static_cast<GLsizei>(cascades.splits.size()),
                     reinterpret_cast<const GLfloat *>(cascades.splits.data()));

        glUniformMatrix4fv(uniformByNameCRC32(uint32_constant<ctcrc32("uCSMSplitSpaceMat")>).location(), 1, GL_FALSE, glm::value_ptr(cascades.splitSpaceMatrix));
    }

    void GLSLDeferredCookTorrance::setExponentialShadowMap(const GLFloatTexture2D& map) {
        setUniformTexture(uint32_constant<ctcrc32("uExponentialShadowMap")>, map);
    }

    void GLSLDeferredCookTorrance::setGridProbesSHTextures(const std::array<GLLDRTexture3D, 4>& textures) {
        setUniformTexture(uint32_constant<ctcrc32("uGridSHMap0")>, textures[0]);
        setUniformTexture(uint32_constant<ctcrc32("uGridSHMap1")>, textures[1]);
        //        setUniformTexture(uint32_constant<ctcrc32("uGridSHMap2")>, textures[2]);
        //        setUniformTexture(uint32_constant<ctcrc32("uGridSHMap3")>, textures[3]);
    }

    void GLSLDeferredCookTorrance::setWorldBoundingBox(const AxisAlignedBox3D& box) {
        glUniformMatrix4fv(uniformByNameCRC32(uint32_constant<ctcrc32("uWorldBoudningBoxTransform")>).location(), 1, GL_FALSE, glm::value_ptr(box.localSpaceMatrix()));
    }

    void GLSLDeferredCookTorrance::setProbePositions(const GLFloat3BufferTexture<glm::vec3>& positions) {
        setUniformTexture(uint32_constant<ctcrc32("uProbePositions")>, positions);
    }

    void GLSLDeferredCookTorrance::setSettings(const RenderingSettings& settings) {
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
