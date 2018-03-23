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

    void GLSLSurfelLighting::setShadowMapsUniforms(const FrustumCascades& cascades, const GLDepthTexture2DArray& shadowMaps) {
        glUniform1i(uniformByNameCRC32(uint32_constant<ctcrc32("uNumberOfCascades")>).location(), cascades.amount);

        glUniformMatrix4fv(uniformByNameCRC32(uint32_constant<ctcrc32("uLightSpaceMatrices[0]")>).location(),
                           static_cast<GLsizei>(cascades.lightViewProjections.size()), GL_FALSE,
                           reinterpret_cast<const GLfloat *>(cascades.lightViewProjections.data()));

        glUniform1fv(uniformByNameCRC32(uint32_constant<ctcrc32("uDepthSplits[0]")>).location(),
                     static_cast<GLsizei>(cascades.splits.size()),
                     reinterpret_cast<const GLfloat *>(cascades.splits.data()));

        setUniformTexture(uint32_constant<ctcrc32("uShadowMapArray")>, shadowMaps);
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

}
