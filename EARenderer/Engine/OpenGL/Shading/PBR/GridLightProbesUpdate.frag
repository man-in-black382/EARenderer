#version 400 core

// Output

layout(location = 0) out vec4 oFragData0;
layout(location = 1) out vec4 oFragData1;
layout(location = 2) out vec4 oFragData2;
layout(location = 3) out vec4 oFragData3;
layout(location = 4) out vec4 oFragData4;
layout(location = 5) out vec4 oFragData5;
layout(location = 6) out vec4 oFragData6;

// Input

in vec2 vTexCoords;
in float vLayer;

// Uniforms

uniform ivec3 uProbesGridResolution;

uniform samplerBuffer uProjectionClusterSphericalHarmonics;
uniform usamplerBuffer uProjectionClusterIndices;
uniform usamplerBuffer uProbeProjectionsMetadata;

uniform sampler2D uSurfelClustersLuminanceMap;

// Types

struct SH {
    vec3 L00;
    vec3 L11;
    vec3 L10;
    vec3 L1_1;
    vec3 L21;
    vec3 L2_1;
    vec3 L2_2;
    vec3 L20;
    vec3 L22;
};

// Functions

SH ZeroSH() {
    SH result;

    result.L00  = vec3(0.0);

    result.L1_1 = vec3(0.0);
    result.L10  = vec3(0.0);
    result.L11  = vec3(0.0);

    result.L2_2 = vec3(0.0);
    result.L2_1 = vec3(0.0);
    result.L21  = vec3(0.0);

    result.L20  = vec3(0.0);

    result.L22  = vec3(0.0);

    return result;
}

SH MultiplySHByColor(SH sh, vec3 color) {
    SH result;

    result.L00  = color * sh.L00;

    result.L1_1 = color * sh.L1_1;
    result.L10  = color * sh.L10;
    result.L11  = color * sh.L11;

    result.L2_2 = color * sh.L2_2;
    result.L2_1 = color * sh.L2_1;
    result.L21  = color * sh.L21;

    result.L20  = color * sh.L20;

    result.L22  = color * sh.L22;

    return result;
}

SH AddTwoSH(SH first, SH second) {
    SH result;

    result.L00  = first.L00  + second.L00;

    result.L1_1 = first.L1_1 + second.L1_1;
    result.L10  = first.L10  + second.L10;
    result.L11  = first.L11  + second.L11;

    result.L2_2 = first.L2_2 + second.L2_2;
    result.L2_1 = first.L2_1 + second.L2_1;
    result.L21  = first.L21  + second.L21;

    result.L20  = first.L20  + second.L20;

    result.L22  = first.L22  + second.L22;

    return result;
}

//
// Unpacks spherical harmonics coefficients
// from the corresponding sample buffer
//
SH UnpackSH(int surfelClusterIndex) {
    SH sh;

    surfelClusterIndex *= 9;

    sh.L00  = vec3(texelFetch(uProjectionClusterSphericalHarmonics, surfelClusterIndex + 0).rgb);
    sh.L11  = vec3(texelFetch(uProjectionClusterSphericalHarmonics, surfelClusterIndex + 1).rgb);
    sh.L10  = vec3(texelFetch(uProjectionClusterSphericalHarmonics, surfelClusterIndex + 2).rgb);
    sh.L1_1 = vec3(texelFetch(uProjectionClusterSphericalHarmonics, surfelClusterIndex + 3).rgb);
    sh.L21  = vec3(texelFetch(uProjectionClusterSphericalHarmonics, surfelClusterIndex + 4).rgb);
    sh.L2_1 = vec3(texelFetch(uProjectionClusterSphericalHarmonics, surfelClusterIndex + 5).rgb);
    sh.L2_2 = vec3(texelFetch(uProjectionClusterSphericalHarmonics, surfelClusterIndex + 6).rgb);
    sh.L20  = vec3(texelFetch(uProjectionClusterSphericalHarmonics, surfelClusterIndex + 7).rgb);
    sh.L22  = vec3(texelFetch(uProjectionClusterSphericalHarmonics, surfelClusterIndex + 8).rgb);

    return sh;
}

// Pack SH coefficients into texels of 7 3D textures
// since minimum of 7 4-component textures are required
// to store 3rd order spherical harmonics for 3 color channels
void PackSHToRenderTargets(SH sh) {
    oFragData0 = vec4(sh.L00.rgb, sh.L11.r);
    oFragData1 = vec4(sh.L11.gb, sh.L10.rg);
    oFragData2 = vec4(sh.L10.b, sh.L1_1.rgb);
    oFragData3 = vec4(sh.L21.rgb, sh.L2_1.r);
    oFragData4 = vec4(sh.L2_1.gb, sh.L2_2.rg);
    oFragData5 = vec4(sh.L2_2.b, sh.L20.rgb);
    oFragData6 = vec4(sh.L22.rgb, 0.0);
}

// Transform normalized texture corrdinates into a 3-dimensional integer index
int FlattenTexCoords() {
    // Texture coordinate interpolation gives us values at texel centers, not edges
    // and so we're accounting for that by adding half a texel size to x and y
    vec3 halfTexel = 1.0 / vec3(uProbesGridResolution) / 2.0;
    vec3 resolution = vec3(uProbesGridResolution - 1);
    float x = (vTexCoords.x + halfTexel.x) * resolution.x;
    float y = (vTexCoords.y + halfTexel.y) * resolution.y;
    // vLayer is not normalized, therefore we're using it as-is
    float z = vLayer;

    int index = uProbesGridResolution.y * uProbesGridResolution.x * int(z) + uProbesGridResolution.x * int(y) + int(x);
    return index;
}

// Schematically, the update of a single light probe on the GPU works like this:
// • Read light probe metadata to determine which projections are used.
// • Loop over all the projections and for every projection:
//
// – Get the SH of the projection.
// – Get the surface group index of the projection.
// – Using the surface group index, get the average luminance of the surface group (calculated in the previous step and stored in an intermediate render target).
// – Add the product of the SH and the luminance to the result SHs.
//
void main() {
    int metadataIndex = FlattenTexCoords() * 2; // Data in uProbeProjectionsMetadata is represented by sequence of offset-length pairs

    uint projectionGroupOffset = texelFetch(uProbeProjectionsMetadata, metadataIndex).r;
    uint projectionGroupSize = texelFetch(uProbeProjectionsMetadata, metadataIndex + 1).r;

    ivec2 luminanceMapSize = textureSize(uSurfelClustersLuminanceMap, 0);
    int luminanceMapWidth = luminanceMapSize.x;
    int luminanceMapHeight = luminanceMapSize.y;

    SH resultingSH = ZeroSH();

    for (uint i = projectionGroupOffset; i < projectionGroupOffset + projectionGroupSize; ++i) {
        uint surfelClusterIndex = texelFetch(uProjectionClusterIndices, int(i)).r;

        vec2 luminanceUV = vec2(float(surfelClusterIndex % luminanceMapWidth) / float(luminanceMapWidth),
                                float(surfelClusterIndex / luminanceMapWidth) / float(luminanceMapHeight));

        vec3 surfelClusterLuminance = texture(uSurfelClustersLuminanceMap, luminanceUV).rgb;

//        surfelClusterLuminance = vec3(3.0);

        SH surfelClusterPrecomputedSH = UnpackSH(int(i));

        SH luminanceSH = MultiplySHByColor(surfelClusterPrecomputedSH, surfelClusterLuminance);

        resultingSH = AddTwoSH(resultingSH, luminanceSH);
    }

    PackSHToRenderTargets(resultingSH);
}
