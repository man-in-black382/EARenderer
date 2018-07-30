//
//  GLSLConeTracing.cpp
//  EARenderer
//
//  Created by Pavel Muratov on 7/30/18.
//  Copyright © 2018 MPO. All rights reserved.
//

#include "GLSLConeTracing.hpp"

namespace EARenderer {

#pragma mark - Lifecycle

    GLSLConeTracing::GLSLConeTracing()
    :
    GLProgram("FullScreenQuad.vert", "SSRConeTracing.frag", "")
    { }

#pragma mark - Lifecycle

    void GLSLConeTracing::setCamera(const Camera& camera) {
        glUniform3fv(uniformByNameCRC32(uint32_constant<ctcrc32("uCameraPosition")>).location(), 1, glm::value_ptr(camera.position()));
        glUniformMatrix4fv(uniformByNameCRC32(uint32_constant<ctcrc32("uCameraViewMat")>).location(), 1, GL_FALSE,
                           glm::value_ptr(camera.viewMatrix()));
        glUniformMatrix4fv(uniformByNameCRC32(uint32_constant<ctcrc32("uCameraProjectionMat")>).location(), 1, GL_FALSE,
                           glm::value_ptr(camera.projectionMatrix()));
        glUniformMatrix4fv(uniformByNameCRC32(uint32_constant<ctcrc32("uCameraViewInverse")>).location(), 1, GL_FALSE,
                           glm::value_ptr(camera.inverseViewMatrix()));
        glUniformMatrix4fv(uniformByNameCRC32(uint32_constant<ctcrc32("uCameraProjectionInverse")>).location(), 1, GL_FALSE,
                           glm::value_ptr(camera.inverseProjectionMatrix()));
    }

    void GLSLConeTracing::setGBuffer(const SceneGBuffer& GBuffer) {
        setUniformTexture(uint32_constant<ctcrc32("uGBufferAlbedoRoughnessMetalnessAONormal")>, GBuffer.albedoRoughnessMetalnessAONormal);
        setUniformTexture(uint32_constant<ctcrc32("uGBufferHiZBuffer")>, GBuffer.HiZBuffer);
        glUniform1i(uniformByNameCRC32(uint32_constant<ctcrc32("uHiZBufferMipCount")>).location(), int32_t(GBuffer.HiZBufferMipCount));
    }

    void GLSLConeTracing::setFrame(const GLFloatTexture2D<GLTexture::Float::RGBA16F>& frame) {
        setUniformTexture(uint32_constant<ctcrc32("uFrame")>, frame);
    }

}
