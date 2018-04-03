//
//  GLSLLightProbeRendering.cpp
//  EARenderer
//
//  Created by Pavel Muratov on 4/2/18.
//  Copyright © 2018 MPO. All rights reserved.
//

#include "GLSLLightProbeRendering.hpp"

#include <glm/gtx/transform.hpp>

namespace EARenderer {

#pragma mark - Lifecycle

    GLSLLightProbeRendering::GLSLLightProbeRendering()
    :
    GLProgram("LightProbeRendering.vert", "LightProbeRendering.frag", "LightProbeRendering.geom")
    { }

#pragma mark - Setters

    void GLSLLightProbeRendering::setCamera(const Camera& camera) {
        glUniformMatrix4fv(uniformByNameCRC32(uint32_constant<ctcrc32("uCameraSpaceMat")>).location(), 1, GL_FALSE, glm::value_ptr(camera.viewProjectionMatrix()));
        glUniform3fv(uniformByNameCRC32(uint32_constant<ctcrc32("uCameraPosition")>).location(), 1, glm::value_ptr(camera.position()));
    }

    void GLSLLightProbeRendering::setGridProbesSHTextures(const std::array<GLHDRTexture3D, 7>& textures) {
        setUniformTexture(uint32_constant<ctcrc32("uGridSHMap0")>, textures[0]);
        setUniformTexture(uint32_constant<ctcrc32("uGridSHMap1")>, textures[1]);
        setUniformTexture(uint32_constant<ctcrc32("uGridSHMap2")>, textures[2]);
        setUniformTexture(uint32_constant<ctcrc32("uGridSHMap3")>, textures[3]);
        setUniformTexture(uint32_constant<ctcrc32("uGridSHMap4")>, textures[4]);
        setUniformTexture(uint32_constant<ctcrc32("uGridSHMap5")>, textures[5]);
        setUniformTexture(uint32_constant<ctcrc32("uGridSHMap6")>, textures[6]);
    }

    void GLSLLightProbeRendering::setWorldBoundingBox(const AxisAlignedBox3D& box) {
        glUniformMatrix4fv(uniformByNameCRC32(uint32_constant<ctcrc32("uWorldBoudningBoxTransform")>).location(), 1, GL_FALSE, glm::value_ptr(box.localSpaceMatrix()));
    }

    void GLSLLightProbeRendering::setSphereRadius(float radius) {
        glUniform1f(uniformByNameCRC32(ctcrc32("uRadius")).location(), radius);
    }

}
