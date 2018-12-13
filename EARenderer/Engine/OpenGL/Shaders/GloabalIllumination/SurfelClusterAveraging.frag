#version 400 core

#include "Constants.glsl"

// Output

out float oAverageLuminance;

// Input

in vec2 vTexCoords;

// Uniforms

uniform usampler2D uSurfelClustersGBuffer;
uniform sampler2D uSurfelsLuminanceMap;

// Functions

uint UnpackSurfelOffset(uint encoded) {
    uint offset = encoded >> 8u;
    return offset;
}

uint UnpackSurfelCount(uint encoded) {
    uint count = encoded & 0xFFu;
    return count;
}

void main() {
    float averagedClusterLuminance = 0.0;

    ivec2 luminanceMapSize = textureSize(uSurfelsLuminanceMap, 0);
    int luminanceMapWidth = luminanceMapSize.x;
    int luminanceMapHeight = luminanceMapSize.y;

    uint clusterMetadata = texture(uSurfelClustersGBuffer, vTexCoords).r;

    uint surfelOffset = UnpackSurfelOffset(clusterMetadata);
    uint surfelCount = UnpackSurfelCount(clusterMetadata);

    for (uint i = surfelOffset; i < surfelOffset + surfelCount; ++i) {
        ivec2 uv = ivec2(i % luminanceMapWidth,
                         i / luminanceMapWidth);

        averagedClusterLuminance += texelFetch(uSurfelsLuminanceMap, uv, 0).r;
    }

    oAverageLuminance = averagedClusterLuminance * HDRNormalizationFactor / surfelCount;
}
