//
//  GLSLLightComposer.cpp
//  EARenderer
//
//  Created by Pavel Muratov on 9/3/18.
//  Copyright © 2018 MPO. All rights reserved.
//

#include "GLSLLightComposer.hpp"

namespace EARenderer {

#pragma mark - Lifecycle

    GLSLLightComposer::GLSLLightComposer()
    :
    GLProgram("FullScreenQuad.vert", "LightComposing.frag", "")
    { }

#pragma mark - Setters

    void GLSLLightComposer::setCamera(const Camera& camera) {
        glUniform3fv(uniformByNameCRC32(uint32_constant<ctcrc32("uCameraPosition")>).location(), 1, glm::value_ptr(camera.position()));
        glUniformMatrix4fv(uniformByNameCRC32(uint32_constant<ctcrc32("uCameraViewInverse")>).location(), 1, GL_FALSE,
                           glm::value_ptr(camera.inverseViewMatrix()));
        glUniformMatrix4fv(uniformByNameCRC32(uint32_constant<ctcrc32("uCameraProjectionInverse")>).location(), 1, GL_FALSE,
                           glm::value_ptr(camera.inverseProjectionMatrix()));
    }

    void GLSLLightComposer::setGBuffer(const SceneGBuffer& GBuffer) {
        setUniformTexture(uint32_constant<ctcrc32("uGBufferAlbedoRoughnessMetalnessAONormal")>, *GBuffer.albedoRoughnessMetalnessAONormal);
        setUniformTexture(uint32_constant<ctcrc32("uGBufferHiZBuffer")>, *GBuffer.HiZBuffer);
    }

    void GLSLLightComposer::setGridProbesSHTextures(const std::array<GLLDRTexture3D, 4>& textures) {
        setUniformTexture(uint32_constant<ctcrc32("uGridSHMap0")>, textures[0]);
        setUniformTexture(uint32_constant<ctcrc32("uGridSHMap1")>, textures[1]);
        setUniformTexture(uint32_constant<ctcrc32("uGridSHMap2")>, textures[2]);
//        setUniformTexture(uint32_constant<ctcrc32("uGridSHMap3")>, textures[3]);
    }

    void GLSLLightComposer::setWorldBoundingBox(const AxisAlignedBox3D& box) {
        glUniformMatrix4fv(uniformByNameCRC32(uint32_constant<ctcrc32("uWorldBoudningBoxTransform")>).location(), 1, GL_FALSE, glm::value_ptr(box.localSpaceMatrix()));
    }

    void GLSLLightComposer::setProbePositions(const GLFloat3BufferTexture<glm::vec3>& positions) {
        setUniformTexture(uint32_constant<ctcrc32("uProbePositions")>, positions);
    }

    void GLSLLightComposer::setLightBuffer(const GLFloatTexture2D<GLTexture::Float::RGBA16F>& lightBuffer) {
        setUniformTexture(uint32_constant<ctcrc32("uLightBuffer")>, lightBuffer);
    }

    void GLSLLightComposer::setReflections(const GLFloatTexture2D<GLTexture::Float::RGBA16F>& reflections) {
        setUniformTexture(uint32_constant<ctcrc32("uReflections")>, reflections);
    }

}
