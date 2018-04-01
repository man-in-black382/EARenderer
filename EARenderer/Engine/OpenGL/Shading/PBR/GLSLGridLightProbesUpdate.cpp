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
    GLProgram("GridLightProbesUpdate.vert", "GridLightProbesUpdate.frag", "GridLightProbesUpdate.geom")
    { }

#pragma mark - Setters

    void GLSLGridLightProbesUpdate::setProbesPerGridDimensionCount(size_t count) {
        glUniform1i(uniformByNameCRC32(uint32_constant<ctcrc32("uProbesPerGridDimensionCount")>).location(), (GLint)count);
    }

    void GLSLGridLightProbesUpdate::setSurfelClustersLuminaceMap(const GLHDRTexture2D& luminanceMap) {
        setUniformTexture(uint32_constant<ctcrc32("uSurfelClustersLuminanceMap")>, luminanceMap);
    }

    void GLSLGridLightProbesUpdate::setProjectionClusterSphericalHarmonics(const GLFloat3BufferTexture<SphericalHarmonics>& SH) {
        setUniformTexture(uint32_constant<ctcrc32("uProjectionClusterSphericalHarmonics")>, SH);
    }

    void GLSLGridLightProbesUpdate::setProjectionClusterIndices(const GLUIntegerBufferTexture<uint32_t>& indices) {
        setUniformTexture(uint32_constant<ctcrc32("uProjectionClusterIndices")>, indices);
    }

    void GLSLGridLightProbesUpdate::setProbeProjectionsMetadata(const GLUIntegerBufferTexture<uint32_t>& metadata) {
        setUniformTexture(uint32_constant<ctcrc32("uProbeProjectionsMetadata")>, metadata);
    }

}
