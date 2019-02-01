//
//  IndirectLightEvaluation.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 04.11.2018.
//  Copyright © 2018 MPO. All rights reserved.
//

#include "GLSLIndirectLightEvaluation.hpp"

namespace EARenderer {

#pragma mark - Lifecycle

    GLSLIndirectLightEvaluation::GLSLIndirectLightEvaluation() : GLProgram("FullScreenQuad.vert", "IndirectLightEvaluation.frag", "") {
    }

#pragma mark - Setters

    void GLSLIndirectLightEvaluation::setCamera(const Camera &camera) {
        glUniform3fv(uniformByNameCRC32(ctcrc32("uCameraPosition")).location(), 1, glm::value_ptr(camera.position()));
        glUniformMatrix4fv(uniformByNameCRC32(ctcrc32("uCameraViewInverse")).location(), 1, GL_FALSE,
                glm::value_ptr(camera.inverseViewMatrix()));
        glUniformMatrix4fv(uniformByNameCRC32(ctcrc32("uCameraProjectionInverse")).location(), 1, GL_FALSE,
                glm::value_ptr(camera.inverseProjectionMatrix()));
    }

    void GLSLIndirectLightEvaluation::setGBuffer(const SceneGBuffer &GBuffer) {
        setUniformTexture(ctcrc32("uMaterialData"), GBuffer.materialData);
        setUniformTexture(ctcrc32("uGBufferHiZBuffer"), GBuffer.HiZBuffer);
    }

    void GLSLIndirectLightEvaluation::setGridProbesSHTextures(const std::array<GLLDRTexture3D, 4> &textures) {
        setUniformTexture(ctcrc32("uGridSHMap0"), textures[0]);
        setUniformTexture(ctcrc32("uGridSHMap1"), textures[1]);
        setUniformTexture(ctcrc32("uGridSHMap2"), textures[2]);
        setUniformTexture(ctcrc32("uGridSHMap3"), textures[3]);
    }

    void GLSLIndirectLightEvaluation::setWorldBoundingBox(const AxisAlignedBox3D &box) {
        glUniformMatrix4fv(uniformByNameCRC32(ctcrc32("uWorldBoudningBoxTransform")).location(), 1, GL_FALSE, glm::value_ptr(box.localSpaceMatrix()));
    }

    void GLSLIndirectLightEvaluation::setProbePositions(const GLFloatBufferTexture<GLTexture::Float::RGB32F, glm::vec3> &positions) {
        setBufferTexture(ctcrc32("uProbePositions"), positions);
    }

    void GLSLIndirectLightEvaluation::setSettings(const RenderingSettings &settings) {
        glUniform1ui(uniformByNameCRC32(ctcrc32("uSettingsBitmask")).location(), settings.meshSettings.booleanBitmask());
    }

}
