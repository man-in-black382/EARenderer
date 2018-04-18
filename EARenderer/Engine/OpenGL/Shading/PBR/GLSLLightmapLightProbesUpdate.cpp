//
//  GLSLGridLightProbesUpdate.cpp
//  EARenderer
//
//  Created by Pavel Muratov on 3/26/18.
//  Copyright © 2018 MPO. All rights reserved.
//

#include "GLSLLightmapLightProbesUpdate.hpp"

namespace EARenderer {

#pragma mark - Lifecycle

    GLSLLightmapLightProbesUpdate::GLSLLightmapLightProbesUpdate()
    :
    GLProgram("LightmapLightProbesUpdate.vert", "LightmapLightProbesUpdate.frag", "")
    { }

#pragma mark - Setters

    void GLSLLightmapLightProbesUpdate::setSurfelClustersLuminaceMap(const GLHDRTexture2D& luminanceMap) {
        setUniformTexture(uint32_constant<ctcrc32("uSurfelClustersLuminanceMap")>, luminanceMap);
    }

    void GLSLLightmapLightProbesUpdate::setLightmapProbeIndicesMap(const GLLDRTexture2D& indicesMap) {
        setUniformTexture(uint32_constant<ctcrc32("uLightmapProbeIndicesMap")>, indicesMap);
    }

    void GLSLLightmapLightProbesUpdate::setProjectionClusterSphericalHarmonics(const GLFloat3BufferTexture<SphericalHarmonics>& SH) {
        setUniformTexture(uint32_constant<ctcrc32("uProjectionClusterSphericalHarmonics")>, SH);
    }

    void GLSLLightmapLightProbesUpdate::setProjectionClusterIndices(const GLUIntegerBufferTexture<uint32_t>& indices) {
        setUniformTexture(uint32_constant<ctcrc32("uProjectionClusterIndices")>, indices);
    }

    void GLSLLightmapLightProbesUpdate::setProbeProjectionsMetadata(const GLUIntegerBufferTexture<uint32_t>& metadata) {
        setUniformTexture(uint32_constant<ctcrc32("uProbeProjectionsMetadata")>, metadata);
    }

}
