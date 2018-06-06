//
//  GLSLZPrepass.cpp
//  EARenderer
//
//  Created by Pavel Muratov on 6/6/18.
//  Copyright © 2018 MPO. All rights reserved.
//

#include "GLSLDepthPrepass.hpp"

namespace EARenderer {

#pragma mark - Lifecycle

    GLSLDepthPrepass::GLSLDepthPrepass()
    :
    GLProgram("DepthPrepass.vert", "DepthPrepass.frag", "")
    { }

#pragma mark - Setters

    void GLSLDepthPrepass::setCamera(const Camera& camera) {
        glUniformMatrix4fv(uniformByNameCRC32(uint32_constant<ctcrc32("uCameraSpaceMat")>).location(), 1, GL_FALSE, glm::value_ptr(camera.viewProjectionMatrix()));
    }

    void GLSLDepthPrepass::setModelMatrix(const glm::mat4& matrix) {
        glUniformMatrix4fv(uniformByNameCRC32(uint32_constant<ctcrc32("uModelMat")>).location(), 1, GL_FALSE, glm::value_ptr(matrix));
    }

}
