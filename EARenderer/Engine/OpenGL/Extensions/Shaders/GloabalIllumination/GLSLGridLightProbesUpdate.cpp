//
//  GLSLGridLightProbesUpdate.cpp
//  EARenderer
//
//  Created by Pavel Muratov on 3/26/18.
//  Copyright © 2018 MPO. All rights reserved.
//

#include "GLSLGridLightProbesUpdate.hpp"

namespace EARenderer {

#pragma mark - Lifecycle

    GLSLGridLightProbesUpdate::GLSLGridLightProbesUpdate()
            :
            GLProgram("GridLightProbesUpdate.vert", "GridLightProbesUpdate.frag", "GridLightProbesUpdate.geom") {
    }

#pragma mark - Setters

    void GLSLGridLightProbesUpdate::setProbesGridResolution(const glm::ivec3 &resolution) {
        glUniform3iv(uniformByNameCRC32(uint32_constant<ctcrc32("uProbesGridResolution")>).location(), 1, glm::value_ptr(resolution));
    }

    void GLSLGridLightProbesUpdate::setSurfelClustersLuminaceMap(const GLFloatTexture2D<GLTexture::Float::R16F> &luminanceMap) {
        setUniformTexture(uint32_constant<ctcrc32("uSurfelClustersLuminanceMap")>, luminanceMap);
    }

    void GLSLGridLightProbesUpdate::setProjectionClusterSphericalHarmonics(const GLFloat3BufferTexture<SphericalHarmonics> &SH) {
        setBufferTexture(uint32_constant<ctcrc32("uProjectionClusterSphericalHarmonics")>, SH);
    }

    void GLSLGridLightProbesUpdate::setSkySphericalHarmonics(const GLFloat3BufferTexture<SphericalHarmonics> &SH) {
        setBufferTexture(uint32_constant<ctcrc32("uSkySphericalHarmonics")>, SH);
    }

    void GLSLGridLightProbesUpdate::setProjectionClusterIndices(const GLUIntegerBufferTexture<uint32_t> &indices) {
        setBufferTexture(uint32_constant<ctcrc32("uProjectionClusterIndices")>, indices);
    }

    void GLSLGridLightProbesUpdate::setProbeProjectionsMetadata(const GLUIntegerBufferTexture<uint32_t> &metadata) {
        setBufferTexture(uint32_constant<ctcrc32("uProbeProjectionsMetadata")>, metadata);
    }

    void GLSLGridLightProbesUpdate::setSkyColorSphericalHarmonics(const SphericalHarmonics &skyColorSH) {
        glUniform3fv(uniformByNameCRC32(ctcrc32("uSkyColorSphericalHarmonics.L00")).location(), 1, (GLfloat *) &skyColorSH.L00());
        glUniform3fv(uniformByNameCRC32(ctcrc32("uSkyColorSphericalHarmonics.L11")).location(), 1, (GLfloat *) &skyColorSH.L11());
        glUniform3fv(uniformByNameCRC32(ctcrc32("uSkyColorSphericalHarmonics.L10")).location(), 1, (GLfloat *) &skyColorSH.L10());
        glUniform3fv(uniformByNameCRC32(ctcrc32("uSkyColorSphericalHarmonics.L1_1")).location(), 1, (GLfloat *) &skyColorSH.L1_1());
        glUniform3fv(uniformByNameCRC32(ctcrc32("uSkyColorSphericalHarmonics.L21")).location(), 1, (GLfloat *) &skyColorSH.L21());
        glUniform3fv(uniformByNameCRC32(ctcrc32("uSkyColorSphericalHarmonics.L2_1")).location(), 1, (GLfloat *) &skyColorSH.L2_1());
        glUniform3fv(uniformByNameCRC32(ctcrc32("uSkyColorSphericalHarmonics.L2_2")).location(), 1, (GLfloat *) &skyColorSH.L2_2());
        glUniform3fv(uniformByNameCRC32(ctcrc32("uSkyColorSphericalHarmonics.L20")).location(), 1, (GLfloat *) &skyColorSH.L20());
        glUniform3fv(uniformByNameCRC32(ctcrc32("uSkyColorSphericalHarmonics.L22")).location(), 1, (GLfloat *) &skyColorSH.L22());
    }

}
