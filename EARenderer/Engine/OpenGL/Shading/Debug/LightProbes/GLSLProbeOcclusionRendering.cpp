//
//  GLSLProbeOcclusionRendering.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 20.05.2018.
//  Copyright © 2018 MPO. All rights reserved.
//

#include "GLSLProbeOcclusionRendering.hpp"

namespace EARenderer {

#pragma mark - Lifecycle

    GLSLProbeOcclusionRendering::GLSLProbeOcclusionRendering()
    :
    GLProgram("ProbeOcclusionRendering.vert", "ProbeOcclusionRendering.frag", "ProbeOcclusionRendering.geom")
    { }

#pragma mark - Setters

    void GLSLProbeOcclusionRendering::setCamera(const Camera& camera) {
        glUniformMatrix4fv(uniformByNameCRC32(uint32_constant<ctcrc32("uCameraSpaceMat")>).location(), 1, GL_FALSE, glm::value_ptr(camera.viewProjectionMatrix()));
    }

    void GLSLProbeOcclusionRendering::setDiffuseProbeOcclusionMapsAtlas(const GLHDRTexture2D& atlas) {
        setUniformTexture(uint32_constant<ctcrc32("uProbeOcclusionMapsAtlas")>, atlas);
    }

}


