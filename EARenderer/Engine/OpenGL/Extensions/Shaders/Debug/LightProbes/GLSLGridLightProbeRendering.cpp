//
//  GLSLLightProbeRendering.cpp
//  EARenderer
//
//  Created by Pavel Muratov on 4/2/18.
//  Copyright © 2018 MPO. All rights reserved.
//

#include "GLSLGridLightProbeRendering.hpp"

#include <glm/gtx/transform.hpp>

namespace EARenderer {

#pragma mark - Lifecycle

    GLSLGridLightProbeRendering::GLSLGridLightProbeRendering()
            :
            GLProgram("GridLightProbeRendering.vert", "GridLightProbeRendering.frag", "GridLightProbeRendering.geom") {
    }

#pragma mark - Setters

    void GLSLGridLightProbeRendering::setCamera(const Camera &camera) {
        glUniformMatrix4fv(uniformByNameCRC32(uint32_constant<ctcrc32("uCameraSpaceMat")>).location(), 1, GL_FALSE, glm::value_ptr(camera.viewProjectionMatrix()));
        glUniform3fv(uniformByNameCRC32(uint32_constant<ctcrc32("uCameraPosition")>).location(), 1, glm::value_ptr(camera.position()));
    }

    void GLSLGridLightProbeRendering::setGridProbesSHTextures(const std::array<GLLDRTexture3D, 4> &textures) {
        setUniformTexture(uint32_constant<ctcrc32("uGridSHMap0")>, textures[0]);
        setUniformTexture(uint32_constant<ctcrc32("uGridSHMap1")>, textures[1]);
        setUniformTexture(uint32_constant<ctcrc32("uGridSHMap2")>, textures[2]);
        setUniformTexture(uint32_constant<ctcrc32("uGridSHMap3")>, textures[3]);
    }

    void GLSLGridLightProbeRendering::setWorldBoundingBox(const AxisAlignedBox3D &box) {
        glUniformMatrix4fv(uniformByNameCRC32(uint32_constant<ctcrc32("uWorldBoudningBoxTransform")>).location(), 1, GL_FALSE, glm::value_ptr(box.localSpaceMatrix()));
    }

    void GLSLGridLightProbeRendering::setProbesGridResolution(const glm::ivec3 &resolution) {
        glUniform3iv(uniformByNameCRC32(uint32_constant<ctcrc32("uProbesGridResolution")>).location(), 1, glm::value_ptr(resolution));
    }

    void GLSLGridLightProbeRendering::setSphereRadius(float radius) {
        glUniform1f(uniformByNameCRC32(ctcrc32("uRadius")).location(), radius);
    }

}
