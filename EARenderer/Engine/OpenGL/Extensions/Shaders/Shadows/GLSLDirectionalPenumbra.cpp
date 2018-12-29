//
//  GLSLDirectionalPenumbra.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 23.11.2018.
//  Copyright © 2018 MPO. All rights reserved.
//

#include "GLSLDirectionalPenumbra.hpp"

namespace EARenderer {

#pragma mark - Lifecycle

    GLSLDirectionalPenumbra::GLSLDirectionalPenumbra()
            : GLProgram("FullScreenQuad.vert", "DirectionalPenumbra.frag", "") {
    }

#pragma mark - Setters

    void GLSLDirectionalPenumbra::setCamera(const Camera &camera) {
        glUniformMatrix4fv(uniformByNameCRC32(ctcrc32("uCameraViewInverse")).location(), 1, GL_FALSE,
                glm::value_ptr(camera.inverseViewMatrix()));
        glUniformMatrix4fv(uniformByNameCRC32(ctcrc32("uCameraProjectionInverse")).location(), 1, GL_FALSE,
                glm::value_ptr(camera.inverseProjectionMatrix()));
    }

    void GLSLDirectionalPenumbra::setGBuffer(const SceneGBuffer &GBuffer) {
        setUniformTexture(ctcrc32("uGBufferHiZBuffer"), *GBuffer.HiZBuffer);
    }

    void GLSLDirectionalPenumbra::setFrustumCascades(const FrustumCascades &cascades) {
        glUniformMatrix4fv(uniformByNameCRC32(ctcrc32("uLightSpaceMatrices[0]")).location(),
                static_cast<GLsizei>(cascades.lightViewProjections.size()), GL_FALSE,
                reinterpret_cast<const GLfloat *>(cascades.lightViewProjections.data()));

        glUniform1i(uniformByNameCRC32(ctcrc32("uDepthSplitsAxis")).location(), static_cast<GLint>(cascades.splitAxis));

        glUniform1fv(uniformByNameCRC32(ctcrc32("uDepthSplits[0]")).location(),
                static_cast<GLsizei>(cascades.splits.size()),
                reinterpret_cast<const GLfloat *>(cascades.splits.data()));

        glUniformMatrix4fv(uniformByNameCRC32(ctcrc32("uCSMSplitSpaceMat")).location(), 1, GL_FALSE, glm::value_ptr(cascades.splitSpaceMatrix));
    }

    void GLSLDirectionalPenumbra::setLight(const DirectionalLight &light) {
        glUniform3fv(uniformByNameCRC32(ctcrc32("uDirectionalLight.direction")).location(), 1, glm::value_ptr(light.direction()));
        glUniform3fv(uniformByNameCRC32(ctcrc32("uDirectionalLight.radiantFlux")).location(), 1, reinterpret_cast<const GLfloat *>(&light.color()));
        glUniform1f(uniformByNameCRC32(ctcrc32("uDirectionalLight.area")).location(), light.area());
    }

    void GLSLDirectionalPenumbra::setDirectionalShadowMapArray(const GLDepthTexture2DArray &array, const GLSampler &bilinearSampler) {
        setUniformTexture(ctcrc32("uDirectionalShadowMapsBilinearSampler"), array, &bilinearSampler);
    }

}
