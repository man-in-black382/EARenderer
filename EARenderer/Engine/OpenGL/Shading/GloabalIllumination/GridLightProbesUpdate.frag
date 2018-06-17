#version 410 core

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

vec3 YCoCg_From_RGB(vec3 rgb) {
    float Co = (rgb.r - rgb.b) / 2.0;
    float t = rgb.b + Co;
    float Cg = (rgb.g - t) / 2.0;
    float Y = t + Cg;

    return vec3(Y, Co, Cg);
}

uint PackSnorm2x16(float first, float second, float range) {
    const float base = 32767.0;

    uint packed = 0;
    uint iFirst = uint(int(first / range * base));
    uint iSecond = uint(int(second / range * base));

    uint firstSignMask = iFirst & (1u << 31); // Leave only sign bit

    uint secondSignMask = iSecond & (1u << 31); // Leave only sign bit
    secondSignMask >>= 16; // Move sign mask by 16 since second value will be stored in LSB of the final uint

    // Move uFirst into MS bits
    packed |= iFirst;
    packed <<= 16;
    packed |= firstSignMask; // Set sign bit

    // Move uSecond into LS bits
    packed |= iSecond & 0x0000FFFFu;
    packed |= secondSignMask; // Set sign bit

    return packed;
}

void WriteSH_311_HalfPacked_ToRenderTargets(SH sh) {
    float maximum = ceil(MaxSHCoefficient(sh));
    uint uMaximum = floatBitsToUint(maximum);

    uint pair0 = PackSnorm2x16(sh.L00.r , sh.L11.r,  maximum);
    uint pair1 = PackSnorm2x16(sh.L10.r , sh.L1_1.r, maximum);
    uint pair2 = PackSnorm2x16(sh.L21.r , sh.L2_1.r, maximum);
    uint pair3 = PackSnorm2x16(sh.L2_2.r, sh.L20.r,  maximum);
    uint pair4 = PackSnorm2x16(sh.L22.r , sh.L00.g,  maximum);
    uint pair5 = PackSnorm2x16(sh.L00.b , 0.0,  maximum);

    oFragData0 = uvec4(pair0, pair1, pair2, pair3);
    oFragData1 = uvec4(pair4, pair5, 0, uMaximum);
}

// Pack SH coefficients into texels of 3D textures
// Use 3 SH bands for first channel and 2 bands for second and third.
void WriteSH_322_HalfPacked_ToRenderTargets(SH sh) {
    float maximum = ceil(MaxSHCoefficient(sh));
    uint uMaximum = floatBitsToUint(maximum);

    uint pair0 = PackSnorm2x16(sh.L00.r , sh.L11.r,  maximum);
    uint pair1 = PackSnorm2x16(sh.L10.r , sh.L1_1.r, maximum);
    uint pair2 = PackSnorm2x16(sh.L21.r , sh.L2_1.r, maximum);
    uint pair3 = PackSnorm2x16(sh.L2_2.r, sh.L20.r,  maximum);
    uint pair4 = PackSnorm2x16(sh.L22.r , sh.L00.g,  maximum);
    uint pair5 = PackSnorm2x16(sh.L11.g , sh.L10.g,  maximum);
    uint pair6 = PackSnorm2x16(sh.L1_1.g, sh.L00.b,  maximum);
    uint pair7 = PackSnorm2x16(sh.L11.b , sh.L10.b,  maximum);
    uint pair8 = PackSnorm2x16(sh.L1_1.b, 0.0, maximum);

    oFragData0 = uvec4(pair0, pair1, pair2, pair3);
    oFragData1 = uvec4(pair4, pair5, pair6, pair7);
    oFragData2 = uvec4(pair8, 0, 0, uMaximum);
}

void WriteSH_333_HalfPacked_ToRenderTargets(SH sh) {
    float maximum = ceil(MaxSHCoefficient(sh));
    uint uMaximum = floatBitsToUint(maximum);

    uint pair0 = PackSnorm2x16(sh.L00.r ,
                               sh.L11.r, maximum);

    uint pair1 = PackSnorm2x16(sh.L10.r ,
                               sh.L1_1.r, maximum);

    uint pair2 = PackSnorm2x16(sh.L21.r ,
                               sh.L2_1.r, maximum);

    uint pair3 = PackSnorm2x16(sh.L2_2.r,
                               sh.L20.r, maximum);

    uint pair4 = PackSnorm2x16(sh.L22.r ,
                               sh.L00.g, maximum);

    uint pair5 = PackSnorm2x16(sh.L11.g ,
                               sh.L10.g, maximum);

    uint pair6 = PackSnorm2x16(sh.L1_1.g ,
                               sh.L21.g, maximum);

    uint pair7 = PackSnorm2x16(sh.L2_1.g ,
                               sh.L2_2.g, maximum);

    uint pair8 = PackSnorm2x16(sh.L20.g,
                               sh.L22.g, maximum);

    uint pair9 = PackSnorm2x16(sh.L00.b ,
                               sh.L11.b, maximum);

    uint pair10 = PackSnorm2x16(sh.L10.b ,
                                sh.L1_1.b, maximum);

    uint pair11 = PackSnorm2x16(sh.L21.b ,
                                sh.L2_1.b, maximum);

    uint pair12 = PackSnorm2x16(sh.L2_2.b,
                                sh.L20.b, maximum);

    uint pair13 = PackSnorm2x16(sh.L22.b ,
                                0.0, maximum);

    oFragData0 = uvec4(pair0, pair1, pair2, pair3);
    oFragData1 = uvec4(pair4, pair5, pair6, pair7);
    oFragData2 = uvec4(pair8, pair9, pair10, pair11);
    oFragData3 = uvec4(pair12, pair13, 0, uMaximum);
}

// Transform normalized texture corrdinates into a 1-dimensional integer index
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

        float luma = 0.2126 * surfelClusterLuminance.r + 0.7152 * surfelClusterLuminance.g + 0.0722 * surfelClusterLuminance.b;

        SH surfelClusterPrecomputedSH = UnpackSH(int(i));

        SH luminanceSH = MultiplySHByColor(surfelClusterPrecomputedSH, vec3(luma));

        resultingSH = AddTwoSH(resultingSH, luminanceSH);
    }

    WriteSH_311_HalfPacked_ToRenderTargets(resultingSH);
}
