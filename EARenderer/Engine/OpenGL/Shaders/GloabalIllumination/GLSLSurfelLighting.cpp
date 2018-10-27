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
    GLProgram("FullScreenQuad.vert", "SurfelLighting.frag", "")
    { }

#pragma mark - Setters

    void GLSLSurfelLighting::setShadowCascades(const FrustumCascades& cascades) {
        glUniformMatrix4fv(uniformByNameCRC32(uint32_constant<ctcrc32("uLightSpaceMatrices[0]")>).location(),
                           static_cast<GLsizei>(cascades.lightViewProjections.size()), GL_FALSE,
                           reinterpret_cast<const GLfloat *>(cascades.lightViewProjections.data()));

        glUniform1i(uniformByNameCRC32(uint32_constant<ctcrc32("uDepthSplitsAxis")>).location(), static_cast<GLint>(cascades.splitAxis));

        glUniform1fv(uniformByNameCRC32(uint32_constant<ctcrc32("uDepthSplits[0]")>).location(),
                     static_cast<GLsizei>(cascades.splits.size()),
                     reinterpret_cast<const GLfloat *>(cascades.splits.data()));

        glUniformMatrix4fv(uniformByNameCRC32(uint32_constant<ctcrc32("uCSMSplitSpaceMat")>).location(), 1, GL_FALSE, glm::value_ptr(cascades.splitSpaceMatrix));
    }

    void GLSLSurfelLighting::setExponentialShadowMap(const GLFloatTexture2D<GLTexture::Float::RGBA32F>& map) {
        setUniformTexture(uint32_constant<ctcrc32("uDirectionalShadowMap")>, map);
    }

    void GLSLSurfelLighting::setLight(const DirectionalLight& light) {
        glUniform3fv(uniformByNameCRC32(uint32_constant<ctcrc32("uDirectionalLight.direction")>).location(), 1, glm::value_ptr(light.direction()));
        glUniform3fv(uniformByNameCRC32(uint32_constant<ctcrc32("uDirectionalLight.radiantFlux")>).location(), 1, reinterpret_cast<const GLfloat *>(&light.color()));
        glUniform1i(uniformByNameCRC32(uint32_constant<ctcrc32("uLightType")>).location(), 0);
    }

    void GLSLSurfelLighting::setLight(const PointLight& light) {
        glUniform3fv(uniformByNameCRC32(uint32_constant<ctcrc32("uPointLight.position")>).location(), 1, glm::value_ptr(light.position()));
        glUniform3fv(uniformByNameCRC32(uint32_constant<ctcrc32("uPointLight.radiantFlux")>).location(), 1, reinterpret_cast<const GLfloat *>(&light.color()));
        glUniform1i(uniformByNameCRC32(uint32_constant<ctcrc32("uLightType")>).location(), 1);
    }

    void GLSLSurfelLighting::setSurfelsGBuffer(const GLHDRTexture2DArray& gBuffer) {
        setUniformTexture(uint32_constant<ctcrc32("uSurfelsGBuffer")>, gBuffer);
    }

    void GLSLSurfelLighting::setGridProbesSHTextures(const std::array<GLLDRTexture3D, 4>& textures) {
        setUniformTexture(uint32_constant<ctcrc32("uGridSHMap0")>, textures[0]);
        setUniformTexture(uint32_constant<ctcrc32("uGridSHMap1")>, textures[1]);
        setUniformTexture(uint32_constant<ctcrc32("uGridSHMap2")>, textures[2]);
        setUniformTexture(uint32_constant<ctcrc32("uGridSHMap3")>, textures[3]);
    }

    void GLSLSurfelLighting::setWorldBoundingBox(const AxisAlignedBox3D& box) {
        glUniformMatrix4fv(uniformByNameCRC32(uint32_constant<ctcrc32("uWorldBoudningBoxTransform")>).location(), 1, GL_FALSE, glm::value_ptr(box.localSpaceMatrix()));
    }

    void GLSLSurfelLighting::setProbePositions(const GLFloat3BufferTexture<glm::vec3>& positions) {
        setUniformTexture(uint32_constant<ctcrc32("uProbePositions")>, positions);
    }

    void GLSLSurfelLighting::setSettings(const RenderingSettings& settings) {
        glUniform1f(uniformByNameCRC32(uint32_constant<ctcrc32("uESMFactor")>).location(), settings.meshSettings.ESMFactor);
        glUniform1i(uniformByNameCRC32(uint32_constant<ctcrc32("uEnableMultibounce")>).location(), settings.meshSettings.lightMultibounceEnabled);
    }

}
