//
//  GLSLLightProbeRendering.cpp
//  EARenderer
//
//  Created by Pavel Muratov on 4/2/18.
//  Copyright © 2018 MPO. All rights reserved.
//

#include "GLSLLightmapLightProbeRendering.hpp"

#include <glm/gtx/transform.hpp>

namespace EARenderer {

#pragma mark - Lifecycle

    GLSLLightmapLightProbeRendering::GLSLLightmapLightProbeRendering()
    :
    GLProgram("LightmapLightProbeRendering.vert", "LightmapLightProbeRendering.frag", "LightmapLightProbeRendering.geom")
    { }

#pragma mark - Setters

    void GLSLLightmapLightProbeRendering::setCamera(const Camera& camera) {
        glUniformMatrix4fv(uniformByNameCRC32(uint32_constant<ctcrc32("uCameraSpaceMat")>).location(), 1, GL_FALSE, glm::value_ptr(camera.viewProjectionMatrix()));
        glUniform3fv(uniformByNameCRC32(uint32_constant<ctcrc32("uCameraPosition")>).location(), 1, glm::value_ptr(camera.position()));
    }

    void GLSLLightmapLightProbeRendering::setLightmapProbesSHTextures(const GLHDRTexture2DArray& textures) {
        setUniformTexture(uint32_constant<ctcrc32("uLightmapSHMaps")>, textures);
    }

    void GLSLLightmapLightProbeRendering::setSphereRadius(float radius) {
        glUniform1f(uniformByNameCRC32(ctcrc32("uRadius")).location(), radius);
    }

}
