//
//  GLSLTriangleRendering.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 30.12.2017.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "GLSLSurfelRendering.hpp"

namespace EARenderer {

#pragma mark - Lifecycle

    GLSLSurfelRendering::GLSLSurfelRendering()
            :
            GLProgram("SurfelRendering.vert", "SurfelRendering.frag", "SurfelRendering.geom") {
    }

#pragma mark - Setters

    void GLSLSurfelRendering::setViewProjectionMatrix(const glm::mat4 &mvp) {
        glUniformMatrix4fv(uniformByNameCRC32(ctcrc32("uViewProjectionMatrix")).location(), 1, GL_FALSE, glm::value_ptr(mvp));
    }

    void GLSLSurfelRendering::setSurfelRadius(float radius) {
        glUniform1f(uniformByNameCRC32(ctcrc32("uRadius")).location(), radius);
    }

    void GLSLSurfelRendering::setShouldUseExternalColor(bool useExternalColor) {
        glUniform1i(uniformByNameCRC32(ctcrc32("uUseExternalColor")).location(), useExternalColor);
    }

    void GLSLSurfelRendering::setExternalColor(const Color &externalColor) {
        glUniform4fv(uniformByNameCRC32(ctcrc32("uExternalColor")).location(), 1, reinterpret_cast<const float *>(&externalColor));
    }

    void GLSLSurfelRendering::setSurfelGroupOffset(int32_t surfelGroupOffset) {
        glUniform1i(uniformByNameCRC32(ctcrc32("uSurfelGroupOffset")).location(), surfelGroupOffset);
    }

    void GLSLSurfelRendering::setSurfelLuminances(const GLFloatTexture2D<GLTexture::Float::R16F> &surfelLuminances) {
        setUniformTexture(ctcrc32("uSurfelLuminances"), surfelLuminances);
    }

    void GLSLSurfelRendering::setSurfelsGBuffer(const GLFloatTexture2DArray<GLTexture::Float::RGB32F> &gBuffer) {
        setUniformTexture(ctcrc32("uSurfelsGBuffer"), gBuffer);
    }

}
