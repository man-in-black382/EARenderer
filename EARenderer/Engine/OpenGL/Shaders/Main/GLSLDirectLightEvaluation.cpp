//
//  GLSLDeferredCookTorrance.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 30.06.2018.
//  Copyright © 2018 MPO. All rights reserved.
//

#include "GLSLDirectLightEvaluation.hpp"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>

namespace EARenderer {

#pragma mark - Lifecycle

    GLSLDirectLightEvaluation::GLSLDirectLightEvaluation()
    :
    GLProgram("FullScreenQuad.vert", "DirectLightEvaluation.frag", "")
    { }

#pragma mark - Setters

    void GLSLDirectLightEvaluation::setCamera(const Camera& camera) {
        glUniform3fv(uniformByNameCRC32(uint32_constant<ctcrc32("uCameraPosition")>).location(), 1, glm::value_ptr(camera.position()));
        glUniformMatrix4fv(uniformByNameCRC32(uint32_constant<ctcrc32("uCameraViewInverse")>).location(), 1, GL_FALSE,
                           glm::value_ptr(camera.inverseViewMatrix()));
        glUniformMatrix4fv(uniformByNameCRC32(uint32_constant<ctcrc32("uCameraProjectionInverse")>).location(), 1, GL_FALSE,
                           glm::value_ptr(camera.inverseProjectionMatrix()));
    }

    void GLSLDirectLightEvaluation::setLight(const PointLight& light) {
        glUniform3fv(uniformByNameCRC32(uint32_constant<ctcrc32("uPointLight.position")>).location(), 1, glm::value_ptr(light.position()));
        glUniform3fv(uniformByNameCRC32(uint32_constant<ctcrc32("uPointLight.radiantFlux")>).location(), 1, reinterpret_cast<const GLfloat *>(&light.color()));
        glUniform1f(uniformByNameCRC32(uint32_constant<ctcrc32("uPointLight.clipDistance")>).location(), light.clipDistance());
        glUniform1i(uniformByNameCRC32(uint32_constant<ctcrc32("uLightType")>).location(), 1);
    }

    void GLSLDirectLightEvaluation::setLight(const DirectionalLight& light) {
        glUniform3fv(uniformByNameCRC32(uint32_constant<ctcrc32("uDirectionalLight.direction")>).location(), 1, glm::value_ptr(light.direction()));
        glUniform3fv(uniformByNameCRC32(uint32_constant<ctcrc32("uDirectionalLight.radiantFlux")>).location(), 1, reinterpret_cast<const GLfloat *>(&light.color()));
        glUniform1i(uniformByNameCRC32(uint32_constant<ctcrc32("uLightType")>).location(), 0);
    }

    void GLSLDirectLightEvaluation::setGBuffer(const SceneGBuffer& GBuffer) {
        setUniformTexture(uint32_constant<ctcrc32("uGBufferAlbedoRoughnessMetalnessAONormal")>, *GBuffer.albedoRoughnessMetalnessAONormal);
        setUniformTexture(uint32_constant<ctcrc32("uGBufferHiZBuffer")>, *GBuffer.HiZBuffer);
    }

    void GLSLDirectLightEvaluation::setFrustumCascades(const FrustumCascades& cascades) {
        glUniformMatrix4fv(uniformByNameCRC32(uint32_constant<ctcrc32("uLightSpaceMatrices[0]")>).location(),
                           static_cast<GLsizei>(cascades.lightViewProjections.size()), GL_FALSE,
                           reinterpret_cast<const GLfloat *>(cascades.lightViewProjections.data()));

        glUniform1i(uniformByNameCRC32(uint32_constant<ctcrc32("uDepthSplitsAxis")>).location(), static_cast<GLint>(cascades.splitAxis));

        glUniform1fv(uniformByNameCRC32(uint32_constant<ctcrc32("uDepthSplits[0]")>).location(),
                     static_cast<GLsizei>(cascades.splits.size()),
                     reinterpret_cast<const GLfloat *>(cascades.splits.data()));

        glUniformMatrix4fv(uniformByNameCRC32(uint32_constant<ctcrc32("uCSMSplitSpaceMat")>).location(), 1, GL_FALSE, glm::value_ptr(cascades.splitSpaceMatrix));
    }

    void GLSLDirectLightEvaluation::setDirectionalShadowMapArray(const GLDepthTexture2DArray& array) {
        setUniformTexture(uint32_constant<ctcrc32("uDirectionalShadowMapArray")>, array);
    }
    
//    void GLSLDirectLightEvaluation::setExponentialShadowMap(const GLFloatTexture2D<GLTexture::Float::RGBA32F>& map) {
//        setUniformTexture(uint32_constant<ctcrc32("uDirectionalShadowMap")>, map);
//    }
//
//    void GLSLDirectLightEvaluation::setExponentialShadowMaps(const GLFloatTextureCubemapArray<GLTexture::Float::R32F>& maps) {
//        setUniformTexture(uint32_constant<ctcrc32("uOmnidirectionalShadowMaps")>, maps);
//    }

    void GLSLDirectLightEvaluation::setShadowMapArrayIndex(size_t index) {
        glUniform1i(uniformByNameCRC32(uint32_constant<ctcrc32("uOmnidirectionalShadowMapIndex")>).location(), static_cast<GLint>(index));
    }

    void GLSLDirectLightEvaluation::setSettings(const RenderingSettings& settings) {
        glUniform1ui(uniformByNameCRC32(uint32_constant<ctcrc32("uSettingsBitmask")>).location(), settings.meshSettings.booleanBitmask());
        //        glUniform1f(uniformByNameCRC32(uint32_constant<ctcrc32("uParallaxMappingStrength")>).location(), settings.meshSettings.parallaxMappingStrength);
//        glUniform1f(uniformByNameCRC32(uint32_constant<ctcrc32("uESMFactor")>).location(), settings.meshSettings.ESMFactor);

        //        int32_t compression = 0;
        //        switch (settings.meshSettings.SHCompression) {
        //            case RenderingSettings::MeshSettings::SphericalHarmonicsCompression::Uncompressed: compression = 0; break;
        //            case RenderingSettings::MeshSettings::SphericalHarmonicsCompression::Compressed322: compression = 1; break;
        //            case RenderingSettings::MeshSettings::SphericalHarmonicsCompression::Compressed311: compression = 2; break;
        //        }
        //        glUniform1i(uniformByNameCRC32(uint32_constant<ctcrc32("uSHCompressionType")>).location(), compression);
    }

}
