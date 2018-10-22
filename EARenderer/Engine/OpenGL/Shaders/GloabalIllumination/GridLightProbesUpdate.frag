#version 410 core

#include "Packing.glsl"
#include "SphericalHarmonics.glsl"
#include "ColorSpace.glsl"

// Output

layout(location = 0) out uvec4 oFragData0;
layout(location = 1) out uvec4 oFragData1;
layout(location = 2) out uvec4 oFragData2;
layout(location = 3) out uvec4 oFragData3;

// Input

in vec2 vTexCoords;
in float vLayer;

// Uniforms

uniform ivec3 uProbesGridResolution;

uniform samplerBuffer uProjectionClusterSphericalHarmonics;
uniform usamplerBuffer uProjectionClusterIndices;
uniform usamplerBuffer uProbeProjectionsMetadata;

uniform sampler2D uSurfelClustersLuminanceMap;

// Functions

float MaxSHCoefficient(SH sh) {
    float maximum = 0.0;

    maximum = max(maximum, max(max(abs(sh.L00.r),  abs(sh.L00.g) ),  abs(sh.L00.b)));
    maximum = max(maximum, max(max(abs(sh.L1_1.r), abs(sh.L1_1.g)),  abs(sh.L1_1.b)));
    maximum = max(maximum, max(max(abs(sh.L10.r),  abs(sh.L10.g) ),  abs(sh.L10.b)));
    maximum = max(maximum, max(max(abs(sh.L11.r),  abs(sh.L11.g) ),  abs(sh.L11.b)));
    maximum = max(maximum, max(max(abs(sh.L2_2.r), abs(sh.L2_2.g)),  abs(sh.L2_2.b)));
    maximum = max(maximum, max(max(abs(sh.L2_1.r), abs(sh.L2_1.g)),  abs(sh.L2_1.b)));
    maximum = max(maximum, max(max(abs(sh.L21.r),  abs(sh.L21.g) ),  abs(sh.L21.b)));
    maximum = max(maximum, max(max(abs(sh.L20.r),  abs(sh.L20.g) ),  abs(sh.L20.b)));
    maximum = max(maximum, max(max(abs(sh.L22.r),  abs(sh.L22.g) ),  abs(sh.L22.b)));

    return maximum;
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

// Packing scheme:
//                         Y    Y      Y    Y       Y    Y       Y     Y      Y   Co
// 9 Luma coefficients  [(L00, L11), (L10, L1_1), (L21, L2_1), (L2_2, L20), (L22, L00),
// 9 Co and 9 Cg coeff.   Cg   Co     Co   Co      Cg   Cg      Cg   Co      Co    Co
// are mixed up          (L00, L11), (L10, L1_1), (L11, L10), (L1_1, L21), (L2_1, L2_2),
//                        Co   Co     Cg   Cg      Cg    Cg     Cg
//                       (L20, L22), (L21, L2_1), (L2_2, L20), (L22, 0.0)]
//
// This packing pattern ensures minimum texture fetches are needed for each compression quality.
// Only 2 texture fetches will be required to obtain 311-spherical harmonics, 3 fetches for 322 and 4 fetches for 333.
//
void PackSHToRenderTargets(SH sh) {
    float maximum = ceil(MaxSHCoefficient(sh));
    uint uMaximum = floatBitsToUint(maximum);

    //                              Y         Y                                                  Y          Y
    uint pair0  = PackSnorm2x16(sh.L00.r,  sh.L11.r,  maximum);  uint pair1  = PackSnorm2x16(sh.L10.r,  sh.L1_1.r, maximum);
    //                              Y         Y                                                  Y          Y
    uint pair2  = PackSnorm2x16(sh.L21.r,  sh.L2_1.r, maximum);  uint pair3  = PackSnorm2x16(sh.L2_2.r, sh.L20.r,  maximum);
    //                              Y         Co                                                 Cg         Co
    uint pair4  = PackSnorm2x16(sh.L22.r,  sh.L00.g,  maximum);  uint pair5  = PackSnorm2x16(sh.L00.b,  sh.L11.g,  maximum);
    //                              Co        Co                                                 Cg         Cg
    uint pair6  = PackSnorm2x16(sh.L10.g,  sh.L1_1.g, maximum);  uint pair7  = PackSnorm2x16(sh.L11.b,  sh.L10.b,  maximum);
    //                              Cg        Co                                                 Co         Co
    uint pair8  = PackSnorm2x16(sh.L1_1.b, sh.L21.g,  maximum);  uint pair9  = PackSnorm2x16(sh.L2_1.g, sh.L2_2.g, maximum);
    //                              Co        Co                                                 Cg         Cg
    uint pair10 = PackSnorm2x16(sh.L20.g,  sh.L22.g,  maximum);  uint pair11 = PackSnorm2x16(sh.L21.b,  sh.L2_1.b, maximum);
    //                              Cg        Cg                                                 Cg
    uint pair12 = PackSnorm2x16(sh.L2_2.b, sh.L20.b,  maximum);  uint pair13 = PackSnorm2x16(sh.L22.b,  0.0,       maximum);

    oFragData0 = uvec4(uMaximum, pair0, pair1, pair2);
    oFragData1 = uvec4(pair3, pair4, pair5, pair6);
    oFragData2 = uvec4(pair7, pair8, pair9, pair10);
    oFragData3 = uvec4(pair11, pair12, pair13, 0);
}

// Transform texture corrdinates into a 1-dimensional integer index
int FlattenTexCoords() {
    ivec3 resolution = uProbesGridResolution;
    float x = gl_FragCoord.x;
    float y = gl_FragCoord.y;
    float z = vLayer;
    return resolution.y * resolution.x * int(z) + resolution.x * int(y) + int(x);
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

        float luma = 0.2126 * surfelClusterLuminance.r + 0.7152 * surfelClusterLuminance.g + 0.0722 * surfelClusterLuminance.b;

        SH surfelClusterPrecomputedSH = UnpackSH(int(i));

        SH luminanceSH = ScaleSH(surfelClusterPrecomputedSH, vec3(luma));

        resultingSH = AddTwoSH(resultingSH, luminanceSH);
    }

    PackSHToRenderTargets(resultingSH);
}
