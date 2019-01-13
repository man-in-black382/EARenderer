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
            GLProgram("FullScreenQuad.vert", "SSRConeTracing.frag", "") {
    }

#pragma mark - Lifecycle

    void GLSLConeTracing::setCamera(const Camera &camera) {
        glUniform3fv(uniformByNameCRC32(ctcrc32("uCameraPosition")).location(), 1, glm::value_ptr(camera.position()));
        glUniformMatrix4fv(uniformByNameCRC32(ctcrc32("uCameraViewInverse")).location(), 1, GL_FALSE,
                glm::value_ptr(camera.inverseViewMatrix()));
        glUniformMatrix4fv(uniformByNameCRC32(ctcrc32("uCameraProjectionInverse")).location(), 1, GL_FALSE,
                glm::value_ptr(camera.inverseProjectionMatrix()));
    }

    void GLSLConeTracing::setGBuffer(const SceneGBuffer &GBuffer) {
        setUniformTexture(ctcrc32("uMaterialData"), GBuffer.materialData);
        setUniformTexture(ctcrc32("uGBufferHiZBuffer"), GBuffer.HiZBuffer);
    }

    void GLSLConeTracing::setReflections(const GLFloatTexture2D<GLTexture::Float::RGBA16F> &reflections) {
        setUniformTexture(ctcrc32("uReflections"), reflections);
        glUniform1i(uniformByNameCRC32(ctcrc32("uMipCount")).location(), int32_t(reflections.mipMapCount()));
    }

    void GLSLConeTracing::setRayHitInfo(const GLFloatTexture2D<GLTexture::Float::RGBA16F> &rayHitInfo) {
        setUniformTexture(ctcrc32("uRayHitInfo"), rayHitInfo);
    }

}
