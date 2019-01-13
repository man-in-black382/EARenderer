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
            GLProgram("FullScreenQuad.vert", "DirectLightEvaluation.frag", "") {
    }

#pragma mark - Setters

    void GLSLDirectLightEvaluation::setCamera(const Camera &camera) {
        glUniform3fv(uniformByNameCRC32(ctcrc32("uCameraPosition")).location(), 1, glm::value_ptr(camera.position()));
        glUniformMatrix4fv(uniformByNameCRC32(ctcrc32("uCameraViewInverse")).location(), 1, GL_FALSE,
                glm::value_ptr(camera.inverseViewMatrix()));
        glUniformMatrix4fv(uniformByNameCRC32(ctcrc32("uCameraProjectionInverse")).location(), 1, GL_FALSE,
                glm::value_ptr(camera.inverseProjectionMatrix()));
    }

    void GLSLDirectLightEvaluation::setLight(const PointLight &light) {
        glUniform3fv(uniformByNameCRC32(ctcrc32("uPointLight.position")).location(), 1, glm::value_ptr(light.position()));
        glUniform3fv(uniformByNameCRC32(ctcrc32("uPointLight.radiantFlux")).location(), 1, reinterpret_cast<const GLfloat *>(&light.color()));
        glUniform1f(uniformByNameCRC32(ctcrc32("uPointLight.nearPlane")).location(), light.nearClipPlane());
        glUniform1f(uniformByNameCRC32(ctcrc32("uPointLight.farPlane")).location(), light.farClipPlane());
        glUniform1f(uniformByNameCRC32(ctcrc32("uPointLight.constant")).location(), light.attenuation.constant);
        glUniform1f(uniformByNameCRC32(ctcrc32("uPointLight.linear")).location(), light.attenuation.linear);
        glUniform1f(uniformByNameCRC32(ctcrc32("uPointLight.quadratic")).location(), light.attenuation.quadratic);
        glUniform1f(uniformByNameCRC32(ctcrc32("uPointLight.area")).location(), light.area());
        glUniform1i(uniformByNameCRC32(ctcrc32("uLightType")).location(), 1);

        auto viewProjections = light.viewProjectionMatrices();
        glUniformMatrix4fv(uniformByNameCRC32(ctcrc32("uPointLight.viewProjections[0]")).location(),
                (GLsizei) viewProjections.size(),
                GL_FALSE,
                (GLfloat *) viewProjections.data());

        auto inverseViewProjections = light.inverseViewProjectionMatrices();
        glUniformMatrix4fv(uniformByNameCRC32(ctcrc32("uPointLight.inverseViewProjections[0]")).location(),
                (GLsizei) inverseViewProjections.size(),
                GL_FALSE,
                (GLfloat *) inverseViewProjections.data());
    }

    void GLSLDirectLightEvaluation::setLight(const DirectionalLight &light) {
        glUniform3fv(uniformByNameCRC32(ctcrc32("uDirectionalLight.direction")).location(), 1, glm::value_ptr(light.direction()));
        glUniform3fv(uniformByNameCRC32(ctcrc32("uDirectionalLight.radiantFlux")).location(), 1, reinterpret_cast<const GLfloat *>(&light.color()));
        glUniform1f(uniformByNameCRC32(ctcrc32("uDirectionalLight.area")).location(), light.area());
        glUniform1i(uniformByNameCRC32(ctcrc32("uLightType")).location(), 0);
    }

    void GLSLDirectLightEvaluation::setGBuffer(const SceneGBuffer &GBuffer) {
        setUniformTexture(ctcrc32("uMaterialData"), GBuffer.materialData);
        setUniformTexture(ctcrc32("uGBufferHiZBuffer"), GBuffer.HiZBuffer);
    }

    void GLSLDirectLightEvaluation::setFrustumCascades(const FrustumCascades &cascades) {
        glUniformMatrix4fv(uniformByNameCRC32(ctcrc32("uLightSpaceMatrices[0]")).location(),
                static_cast<GLsizei>(cascades.lightViewProjections.size()), GL_FALSE,
                reinterpret_cast<const GLfloat *>(cascades.lightViewProjections.data()));

        glUniform1i(uniformByNameCRC32(ctcrc32("uDepthSplitsAxis")).location(), static_cast<GLint>(cascades.splitAxis));

        glUniform1fv(uniformByNameCRC32(ctcrc32("uDepthSplits[0]")).location(),
                static_cast<GLsizei>(cascades.splits.size()),
                reinterpret_cast<const GLfloat *>(cascades.splits.data()));

        glUniformMatrix4fv(uniformByNameCRC32(ctcrc32("uCSMSplitSpaceMat")).location(), 1, GL_FALSE, glm::value_ptr(cascades.splitSpaceMatrix));
    }

    void GLSLDirectLightEvaluation::setDirectionalShadowMapArray(const GLDepthTexture2DArray &array) {
        setUniformTexture(ctcrc32("uDirectionalShadowMapsComparisonSampler"), array);
    }

    void GLSLDirectLightEvaluation::setOmnidirectionalShadowCubemap(const GLDepthTextureCubemap &cubemap) {
        setUniformTexture(ctcrc32("uOmnidirectionalShadowMapComparisonSampler"), cubemap);
    }

    void GLSLDirectLightEvaluation::setPenumbra(const GLFloatTexture2D<GLTexture::Float::R16F> &penumbra) {
        setUniformTexture(ctcrc32("uPenumbra"), penumbra);
    }

    void GLSLDirectLightEvaluation::setSettings(const RenderingSettings &settings) {
        glUniform1ui(uniformByNameCRC32(ctcrc32("uSettingsBitmask")).location(), settings.meshSettings.booleanBitmask());
        //        glUniform1f(uniformByNameCRC32(ctcrc32("uParallaxMappingStrength")).location(), settings.meshSettings.parallaxMappingStrength);
    }

}
