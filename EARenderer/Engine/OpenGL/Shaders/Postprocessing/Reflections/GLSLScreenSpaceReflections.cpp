//
//  GLSLScreenSpaceReflections.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 15.07.2018.
//  Copyright © 2018 MPO. All rights reserved.
//

#include "GLSLScreenSpaceReflections.hpp"

namespace EARenderer {

#pragma mark - Lifecycle

    GLSLScreenSpaceReflections::GLSLScreenSpaceReflections()
            :
            GLProgram("FullScreenQuad.vert", "SSR.frag", "") {
    }

#pragma mark - Lifecycle

    void GLSLScreenSpaceReflections::setCamera(const Camera &camera) {
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

    void GLSLScreenSpaceReflections::setGBuffer(const SceneGBuffer &GBuffer) {
        setUniformTexture(uint32_constant<ctcrc32("uMaterialData")>, *GBuffer.materialData);
        setUniformTexture(uint32_constant<ctcrc32("uGBufferHiZBuffer")>, *GBuffer.HiZBuffer);
        glUniform1i(uniformByNameCRC32(uint32_constant<ctcrc32("uHiZBufferMipCount")>).location(), int32_t(GBuffer.HiZBufferMipCount));
    }

}

