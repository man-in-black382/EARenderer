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

    void GLSLConeTracing::setGBuffer(const SceneGBuffer& GBuffer) {
        setUniformTexture(uint32_constant<ctcrc32("uGBufferAlbedoRoughnessMetalnessAONormal")>, *GBuffer.albedoRoughnessMetalnessAONormal);
        setUniformTexture(uint32_constant<ctcrc32("uGBufferHiZBuffer")>, *GBuffer.HiZBuffer);
    }

    void GLSLConeTracing::setReflections(const GLFloatTexture2D<GLTexture::Float::RGBA16F>& reflections) {
        setUniformTexture(uint32_constant<ctcrc32("uReflections")>, reflections);
        glUniform1i(uniformByNameCRC32(uint32_constant<ctcrc32("uMipCount")>).location(), int32_t(reflections.mipMapCount()));
    }

    void GLSLConeTracing::setRayHitInfo(const GLFloatTexture2D<GLTexture::Float::RGBA16F>& rayHitInfo) {
        setUniformTexture(uint32_constant<ctcrc32("uRayHitInfo")>, rayHitInfo);
    }

}
