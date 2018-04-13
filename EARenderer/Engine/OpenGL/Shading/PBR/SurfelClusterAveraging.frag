#version 400 core

// Output

out vec4 oFragColor;

// Input

in vec2 vTexCoords;

// Uniforms

uniform usampler2D uSurfelClustersGBuffer;
uniform sampler2D uSurfelsLuminanceMap;

// Functions

uint UnpackSurfelOffset(uvec4 texel) {
    uint offset = 0;
    offset |= texel.r;
    offset <<= 8;
    offset |= texel.g;
    offset <<= 8;
    offset |= texel.b;
    return offset;
}

uint UnpackSurfelCount(uvec4 texel) {
    return texel.a;
}

void main() {
    vec3 averagedClusterLuminance = vec3(0.0);

    ivec2 luminanceMapSize = textureSize(uSurfelsLuminanceMap, 0);
    int luminanceMapWidth = luminanceMapSize.x;
    int luminanceMapHeight = luminanceMapSize.y;

    uvec4 clusterMetadata = texture(uSurfelClustersGBuffer, vTexCoords);

    uint surfelOffset = UnpackSurfelOffset(clusterMetadata);
    uint surfelCount = UnpackSurfelCount(clusterMetadata);

    for (uint i = surfelOffset; i < surfelOffset + surfelCount; ++i) {
        vec2 uv = vec2(float(i % luminanceMapWidth) / luminanceMapWidth,
                       float(i / luminanceMapWidth) / luminanceMapHeight);

        averagedClusterLuminance += texture(uSurfelsLuminanceMap, uv).rgb;
    }

    averagedClusterLuminance /= surfelCount;

    oFragColor = vec4(averagedClusterLuminance, 1.0);
}
