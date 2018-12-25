//
//  GLSLSurfelLighting.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 21.03.2018.
//  Copyright © 2018 MPO. All rights reserved.
//

#include "GLSLSurfelLighting.hpp"

namespace EARenderer {

#pragma mark - Lifecycle

    GLSLSurfelLighting::GLSLSurfelLighting()
            :
            GLProgram("FullScreenQuad.vert", "SurfelLighting.frag", "") {
    }

#pragma mark - Setters

    void GLSLSurfelLighting::setShadowCascades(const FrustumCascades &cascades) {
        glUniformMatrix4fv(uniformByNameCRC32(ctcrc32("uLightSpaceMatrices[0]")).location(),
                static_cast<GLsizei>(cascades.lightViewProjections.size()), GL_FALSE,
                reinterpret_cast<const GLfloat *>(cascades.lightViewProjections.data()));

        glUniform1i(uniformByNameCRC32(ctcrc32("uDepthSplitsAxis")).location(), static_cast<GLint>(cascades.splitAxis));

        glUniform1fv(uniformByNameCRC32(ctcrc32("uDepthSplits[0]")).location(),
                static_cast<GLsizei>(cascades.splits.size()),
                reinterpret_cast<const GLfloat *>(cascades.splits.data()));

        glUniformMatrix4fv(uniformByNameCRC32(ctcrc32("uCSMSplitSpaceMat")).location(), 1, GL_FALSE, glm::value_ptr(cascades.splitSpaceMatrix));
    }

    void GLSLSurfelLighting::setDirectionalShadowMapArray(const GLDepthTexture2DArray &array) {
        setUniformTexture(ctcrc32("uDirectionalShadowMapArray"), array);
    }

    void GLSLSurfelLighting::setLight(const PointLight &light) {
        glUniform3fv(uniformByNameCRC32(ctcrc32("uPointLight.position")).location(), 1, glm::value_ptr(light.position()));
        glUniform3fv(uniformByNameCRC32(ctcrc32("uPointLight.radiantFlux")).location(), 1, reinterpret_cast<const GLfloat *>(&light.color()));
        glUniform1f(uniformByNameCRC32(ctcrc32("uPointLight.nearPlane")).location(), light.nearClipPlane());
        glUniform1f(uniformByNameCRC32(ctcrc32("uPointLight.farPlane")).location(), light.farClipPlane());
        glUniform1f(uniformByNameCRC32(ctcrc32("uPointLight.constant")).location(), light.attenuation.constant);
        glUniform1f(uniformByNameCRC32(ctcrc32("uPointLight.linear")).location(), light.attenuation.linear);
        glUniform1f(uniformByNameCRC32(ctcrc32("uPointLight.quadratic")).location(), light.attenuation.quadratic);
        glUniform1f(uniformByNameCRC32(ctcrc32("uPointLight.area")).location(), light.area());
        glUniform1i(uniformByNameCRC32(ctcrc32("uLightType")).location(), 1);
    }

    void GLSLSurfelLighting::setLight(const DirectionalLight &light) {
        glUniform3fv(uniformByNameCRC32(ctcrc32("uDirectionalLight.direction")).location(), 1, glm::value_ptr(light.direction()));
        glUniform3fv(uniformByNameCRC32(ctcrc32("uDirectionalLight.radiantFlux")).location(), 1, reinterpret_cast<const GLfloat *>(&light.color()));
        glUniform1f(uniformByNameCRC32(ctcrc32("uDirectionalLight.area")).location(), light.area());
        glUniform1i(uniformByNameCRC32(ctcrc32("uLightType")).location(), 0);
    }

    void GLSLSurfelLighting::setSurfelsGBuffer(const GLFloatTexture2DArray<GLTexture::Float::RGB32F> &gBuffer) {
        setUniformTexture(ctcrc32("uSurfelsGBuffer"), gBuffer);
    }

    void GLSLSurfelLighting::setGridProbesSHTextures(const std::array<GLLDRTexture3D, 4> &textures) {
        setUniformTexture(ctcrc32("uGridSHMap0"), textures[0]);
        setUniformTexture(ctcrc32("uGridSHMap1"), textures[1]);
        setUniformTexture(ctcrc32("uGridSHMap2"), textures[2]);
        setUniformTexture(ctcrc32("uGridSHMap3"), textures[3]);
    }

    void GLSLSurfelLighting::setWorldBoundingBox(const AxisAlignedBox3D &box) {
        glUniformMatrix4fv(uniformByNameCRC32(ctcrc32("uWorldBoudningBoxTransform")).location(), 1, GL_FALSE, glm::value_ptr(box.localSpaceMatrix()));
    }

    void GLSLSurfelLighting::setProbePositions(const GLFloat3BufferTexture<glm::vec3> &positions) {
        setBufferTexture(ctcrc32("uProbePositions"), positions);
    }

    void GLSLSurfelLighting::setOmnidirectionalShadowMap(const GLDepthTextureCubemap &shadowMap) {
        setUniformTexture(ctcrc32("uOmnidirectionalShadowMap"), shadowMap);
    }

    void GLSLSurfelLighting::setSettings(const RenderingSettings &settings) {
        glUniform1i(uniformByNameCRC32(ctcrc32("uEnableMultibounce")).location(), settings.meshSettings.lightMultibounceEnabled);
    }

}
