//
//  GLSLLightProbeLinksRendering.cpp
//  EARenderer
//
//  Created by Pavel Muratov on 4/3/18.
//  Copyright © 2018 MPO. All rights reserved.
//

#include "GLSLLightProbeLinksRendering.hpp"

namespace EARenderer {

#pragma mark - Lifecycle

    GLSLLightProbeLinksRendering::GLSLLightProbeLinksRendering()
    :
    GLProgram("LightProbeLinksRendering.vert", "LightProbeLinksRendering.frag", "LightProbeLinksRendering.geom")
    { }

#pragma mark - Setters

    void GLSLLightProbeLinksRendering::setCamera(const Camera& camera) {
        glUniformMatrix4fv(uniformByNameCRC32(uint32_constant<ctcrc32("uCameraSpaceMat")>).location(), 1, GL_FALSE, glm::value_ptr(camera.viewProjectionMatrix()));
    }

    void GLSLLightProbeLinksRendering::setWorldBoundingBox(const AxisAlignedBox3D& box) {
        glUniformMatrix4fv(uniformByNameCRC32(uint32_constant<ctcrc32("uWorldBoudningBoxTransform")>).location(), 1, GL_FALSE, glm::value_ptr(box.localSpaceMatrix()));
    }

    void GLSLLightProbeLinksRendering::setProjectionClusterIndices(const GLUIntegerBufferTexture<uint32_t>& indices) {
        setUniformTexture(uint32_constant<ctcrc32("uProjectionClusterIndices")>, indices);
    }

    void GLSLLightProbeLinksRendering::setProbeProjectionsMetadata(const GLUIntegerBufferTexture<uint32_t>& metadata) {
        setUniformTexture(uint32_constant<ctcrc32("uProbeProjectionsMetadata")>, metadata);
    }

    void GLSLLightProbeLinksRendering::setSurfelClusterCenters(const GLFloat3BufferTexture<glm::vec3>& centers) {
        setUniformTexture(uint32_constant<ctcrc32("uClusterCenters")>, centers);
    }

}
