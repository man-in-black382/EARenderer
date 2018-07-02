#version 400 core

// Constants

const float PI = 3.1415926535897932384626433832795;
const int kMaxCascades = 4;

const int kLightTypeDirectional = 0;
const int kLightTypePoint       = 1;
const int kLightTypeSpot        = 2;

const int kSHCompression333 = 0;
const int kSHCompression322 = 1;
const int kSHCompression311 = 2;

// Spherical harmonics
const float kC1 = 0.429043;
const float kC2 = 0.511664;
const float kC3 = 0.743125;
const float kC4 = 0.886227;
const float kC5 = 0.247708;

// Output

layout(location = 0) out vec4 oBaseOutput;
layout(location = 1) out vec4 oBrightOutput;

// Input

in vec2 vTexCoords;

// Uniforms

struct DirectionalLight {
    vec3 radiantFlux; // a.k.a color
    vec3 direction;
};

struct PointLight {
    vec3 radiantFlux; // a.k.a color
    vec3 position;
};

struct Spotlight {
    vec3 radiantFlux; // a.k.a color
    vec3 position;
    vec3 direction;
    float cutOffAngleCos;
};

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

struct vec8 {
    float value0; float value1; float value2; float value3;
    float value4; float value5; float value6; float value7;
};

uniform sampler2DArray uGBuffer;

uniform vec3 uCameraPosition;
uniform mat4 uCameraViewMat;
uniform mat4 uCameraViewInverse;
uniform mat4 uCameraProjectionInverse;
uniform mat4 uWorldBoudningBoxTransform;

uniform DirectionalLight uDirectionalLight;
uniform PointLight uPointLight;
uniform Spotlight uSpotlight;

uniform int uLightType;
uniform uint uSettingsBitmask;
uniform float uParallaxMappingStrength;
uniform int uSHCompressionType;

// Shadow mapping
uniform mat4 uCSMSplitSpaceMat;
uniform mat4 uLightSpaceMatrices[kMaxCascades];
uniform float uDepthSplits[kMaxCascades];
uniform int uNumberOfCascades;

uniform float uESMFactor;
uniform sampler2D uExponentialShadowMap;

// Shperical harmonics

uniform usampler3D uGridSHMap0;
uniform usampler3D uGridSHMap1;
uniform usampler3D uGridSHMap2;
uniform usampler3D uGridSHMap3;

uniform samplerBuffer uProbePositions;

// Functions

// Shrink tex coords by the size of 1 texel, which will result in a (0; 0; 0)
// coordinate to become (0.5; 0.5; 0.5) coordinate (in texel space)
vec3 AlignWithTexelCenters(vec3 texCoords) {
    ivec3 gridResolution = textureSize(uGridSHMap0, 0);
    vec3 halfTexel = 1.0 / vec3(gridResolution) / 2.0;
    vec3 reductionFactor = vec3(gridResolution - 1) / vec3(gridResolution);
    return texCoords * reductionFactor + halfTexel;
}

// Settings
bool areMaterialsEnabled()          { return bool((uSettingsBitmask >> 4u) & 1u); }
bool isGlobalIlluminationEnabled()  { return bool((uSettingsBitmask >> 3u) & 1u); }
bool isLightMultibounceEnabled()    { return bool((uSettingsBitmask >> 2u) & 1u); }
bool isMeshRenderingEnabled()       { return bool((uSettingsBitmask >> 1u) & 1u); }
bool isParallaxMappingEnabled()     { return bool((uSettingsBitmask >> 0u) & 1u); }

////////////////////////////////////////////////////////////
/////////////////// Spherical harmonics ////////////////////
////////////////////////////////////////////////////////////

SH ZeroSH() {
    SH result;
    result.L00  = vec3(0.0); result.L1_1 = vec3(0.0); result.L10  = vec3(0.0);
    result.L11  = vec3(0.0); result.L2_2 = vec3(0.0); result.L2_1 = vec3(0.0);
    result.L21  = vec3(0.0); result.L20  = vec3(0.0); result.L22  = vec3(0.0);
    return result;
}

SH ScaleSH(SH sh, vec3 scale) {
    SH result;
    result.L00  = scale * sh.L00; result.L1_1 = scale * sh.L1_1; result.L10  = scale * sh.L10;
    result.L11  = scale * sh.L11; result.L2_2 = scale * sh.L2_2; result.L2_1 = scale * sh.L2_1;
    result.L21  = scale * sh.L21; result.L20  = scale * sh.L20;  result.L22  = scale * sh.L22;
    return result;
}

SH SumSH(SH first, SH second, SH third, SH fourth, SH fifth, SH sixth, SH seventh, SH eighth) {
    SH result;
    result.L00  = first.L00 + second.L00 + third.L00 + fourth.L00 + fifth.L00 + sixth.L00 + seventh.L00 + eighth.L00;
    result.L1_1 = first.L1_1 + second.L1_1 + third.L1_1 + fourth.L1_1 + fifth.L1_1 + sixth.L1_1 + seventh.L1_1 + eighth.L1_1;
    result.L10  = first.L10 + second.L10 + third.L10 + fourth.L10 + fifth.L10 + sixth.L10 + seventh.L10 + eighth.L10;
    result.L11  = first.L11 + second.L11 + third.L11 + fourth.L11 + fifth.L11 + sixth.L11 + seventh.L11 + eighth.L11;
    result.L2_2 = first.L2_2 + second.L2_2 + third.L2_2 + fourth.L2_2 + fifth.L2_2 + sixth.L2_2 + seventh.L2_2 + eighth.L2_2;
    result.L2_1 = first.L2_1 + second.L2_1 + third.L2_1 + fourth.L2_1 + fifth.L2_1 + sixth.L2_1 + seventh.L2_1 + eighth.L2_1;
    result.L21  = first.L21 + second.L21 + third.L21 + fourth.L21 + fifth.L21 + sixth.L21 + seventh.L21 + eighth.L21;
    result.L20  = first.L20 + second.L20 + third.L20 + fourth.L20 + fifth.L20 + sixth.L20 + seventh.L20 + eighth.L20;
    result.L22  = first.L22 + second.L22 + third.L22 + fourth.L22 + fifth.L22 + sixth.L22 + seventh.L22 + eighth.L22;
    return result;
}

vec3 RGB_From_YCoCg(vec3 YCoCg) {
    float t = YCoCg.x - YCoCg.z;
    float g = YCoCg.x + YCoCg.z;
    float b = t - YCoCg.y;
    float r = t + YCoCg.y;

    return vec3(r, g, b);
}

vec2 UnpackSnorm2x16(uint package, float range) {
    const float base = 32767.0;

    // Unpack encoded floats into individual variables
    uint uFirst = package >> 16;
    uint uSecond = package & 0x0000FFFFu;

    // Extract sign bits
    uint firstSignMask = uFirst & (1u << 15);
    uint secondSignMask = uSecond & (1u << 15);

    // If sign bit indicates negativity, fill MS 16 bits with 1s
    uFirst |= firstSignMask != 0 ? 0xFFFF0000u : 0x0u;
    uSecond |= secondSignMask != 0 ? 0xFFFF0000u : 0x0u;

    // Now get signed integer representation
    int iFirst = int(uFirst);
    int iSecond = int(uSecond);

    // At last, convert integers back to floats using range and base
    float fFirst = (float(iFirst) / base) * range;
    float fSecond = (float(iSecond) / base) * range;

    return vec2(fFirst, fSecond);
}

// Packing scheme:
//                         Y    Y      Y    Y       Y    Y       Y     Y      Y   Co
// 9 Luma coefficients  [(L00, L11), (L10, L1_1), (L21, L2_1), (L2_2, L20), (L22, L00),
// 9 Co and 9 Cg coeff.   Cg   Co     Co   Co      Cg   Cg      Cg   Co      Co    Co
// are mixed up          (L00, L11), (L10, L1_1), (L11, L10), (L1_1, L21), (L2_1, L2_2),
//                        Co   Co     Cg   Cg      Cg    Cg     Cg
//                       (L20, L22), (L21, L2_1), (L2_2, L20), (L22, 0.0)]
//
SH UnpackSH_333(vec3 texCoords) {
    SH sh = ZeroSH();

    ivec3 iTexCoords = ivec3(texCoords);

    uvec4 shMap0Data = texelFetch(uGridSHMap0, iTexCoords, 0);
    uvec4 shMap1Data = texelFetch(uGridSHMap1, iTexCoords, 0);
    uvec4 shMap2Data = texelFetch(uGridSHMap2, iTexCoords, 0);
    uvec4 shMap3Data = texelFetch(uGridSHMap3, iTexCoords, 0);

    float range = uintBitsToFloat(shMap0Data.r);

    vec2 pair0 = UnpackSnorm2x16(shMap0Data.g, range);  vec2 pair1 = UnpackSnorm2x16(shMap0Data.b, range);
    vec2 pair2 = UnpackSnorm2x16(shMap0Data.a, range);  vec2 pair3 = UnpackSnorm2x16(shMap1Data.r, range);
    vec2 pair4 = UnpackSnorm2x16(shMap1Data.g, range);  vec2 pair5 = UnpackSnorm2x16(shMap1Data.b, range);
    vec2 pair6 = UnpackSnorm2x16(shMap1Data.a, range);  vec2 pair7 = UnpackSnorm2x16(shMap2Data.r, range);
    vec2 pair8 = UnpackSnorm2x16(shMap2Data.g, range);  vec2 pair9 = UnpackSnorm2x16(shMap2Data.b, range);
    vec2 pair10 = UnpackSnorm2x16(shMap2Data.a, range); vec2 pair11 = UnpackSnorm2x16(shMap3Data.r, range);
    vec2 pair12 = UnpackSnorm2x16(shMap3Data.g, range); vec2 pair13 = UnpackSnorm2x16(shMap3Data.b, range);

    sh.L00.r = pair0.x;   sh.L11.r = pair0.y;   sh.L10.r = pair1.x;   sh.L1_1.r = pair1.y;
    sh.L21.r = pair2.x;   sh.L2_1.r = pair2.y;  sh.L2_2.r = pair3.x;  sh.L20.r = pair3.y;
    sh.L22.r = pair4.x;   sh.L00.g = pair4.y;   sh.L00.b = pair5.x;   sh.L11.g = pair5.y;
    sh.L10.g = pair6.x,   sh.L1_1.g = pair6.y;  sh.L11.b = pair7.x;   sh.L10.b = pair7.y;
    sh.L1_1.b = pair8.x;  sh.L21.g = pair8.y;   sh.L2_1.g = pair9.x;  sh.L2_2.g = pair9.y;
    sh.L20.g = pair10.x,  sh.L22.g = pair10.y;  sh.L21.b = pair11.x,  sh.L2_1.b = pair11.y;
    sh.L2_2.b = pair12.x; sh.L20.b = pair12.y;  sh.L22.b = pair13.x;

    return sh;
}

SH UnpackSH_322(vec3 texCoords) {
    SH sh = ZeroSH();

    ivec3 iTexCoords = ivec3(texCoords);

    uvec4 shMap0Data = texelFetch(uGridSHMap0, iTexCoords, 0);
    uvec4 shMap1Data = texelFetch(uGridSHMap1, iTexCoords, 0);
    uvec4 shMap2Data = texelFetch(uGridSHMap2, iTexCoords, 0);

    float range = uintBitsToFloat(shMap0Data.r);

    vec2 pair0 = UnpackSnorm2x16(shMap0Data.g, range);  vec2 pair1 = UnpackSnorm2x16(shMap0Data.b, range);
    vec2 pair2 = UnpackSnorm2x16(shMap0Data.a, range);  vec2 pair3 = UnpackSnorm2x16(shMap1Data.r, range);
    vec2 pair4 = UnpackSnorm2x16(shMap1Data.g, range);  vec2 pair5 = UnpackSnorm2x16(shMap1Data.b, range);
    vec2 pair6 = UnpackSnorm2x16(shMap1Data.a, range);  vec2 pair7 = UnpackSnorm2x16(shMap2Data.r, range);
    vec2 pair8 = UnpackSnorm2x16(shMap2Data.g, range);

    sh.L00.r = pair0.x;   sh.L11.r = pair0.y;   sh.L10.r = pair1.x;   sh.L1_1.r = pair1.y;
    sh.L21.r = pair2.x;   sh.L2_1.r = pair2.y;  sh.L2_2.r = pair3.x;  sh.L20.r = pair3.y;
    sh.L22.r = pair4.x;   sh.L00.g = pair4.y;   sh.L00.b = pair5.x;   sh.L11.g = pair5.y;
    sh.L10.g = pair6.x,   sh.L1_1.g = pair6.y;  sh.L11.b = pair7.x;   sh.L10.b = pair7.y;
    sh.L1_1.b = pair8.x;

    return sh;
}

SH UnpackSH_311(vec3 texCoords) {
    SH sh = ZeroSH();

    ivec3 iTexCoords = ivec3(texCoords);

    uvec4 shMap0Data = texelFetch(uGridSHMap0, iTexCoords, 0);
    uvec4 shMap1Data = texelFetch(uGridSHMap1, iTexCoords, 0);

    float range = uintBitsToFloat(shMap0Data.r);

    vec2 pair0 = UnpackSnorm2x16(shMap0Data.g, range);  vec2 pair1 = UnpackSnorm2x16(shMap0Data.b, range);
    vec2 pair2 = UnpackSnorm2x16(shMap0Data.a, range);  vec2 pair3 = UnpackSnorm2x16(shMap1Data.r, range);
    vec2 pair4 = UnpackSnorm2x16(shMap1Data.g, range);  vec2 pair5 = UnpackSnorm2x16(shMap1Data.b, range);

    sh.L00.r = pair0.x;   sh.L11.r = pair0.y;   sh.L10.r = pair1.x;   sh.L1_1.r = pair1.y;
    sh.L21.r = pair2.x;   sh.L2_1.r = pair2.y;  sh.L2_2.r = pair3.x;  sh.L20.r = pair3.y;
    sh.L22.r = pair4.x;   sh.L00.g = pair4.y;   sh.L00.b = pair5.x;

    return sh;
}

SH UnpackSH(vec3 texCoords) {
//    switch (uSHCompressionType) {
//        case kSHCompression333: return UnpackSH_333(texCoords);
//        case kSHCompression322: return UnpackSH_322(texCoords);
//        case kSHCompression311: return UnpackSH_311(texCoords);
//        default: return UnpackSH_333(texCoords);
//    }
    return UnpackSH_322(texCoords);
}

float ProbeOcclusionFactor(vec3 probeGridPos, ivec3 gridSize, vec3 fragNorm, vec3 fragWorldPos) {
    // [x + WIDTH * (y + HEIGHT * z)]
    int gridWidth = gridSize.x;
    int gridHeight = gridSize.y;
    int probeCoord1D = int(probeGridPos.x) + gridWidth * (int(probeGridPos.y) + gridHeight * int(probeGridPos.z));

    vec3 probePosition = texelFetch(uProbePositions, probeCoord1D).xyz;

    vec3 fragToProbeVector = normalize(probePosition - fragWorldPos);
    float weight = max(0.0, dot(fragToProbeVector, fragNorm));
    return weight;
}

vec8 TriLerp(vec3 pMin, vec3 pMax, vec3 p) {
    //
    //        5-------6
    //       /|      /|
    // Y    / |     / |
    // ^   1--|----2  |
    // |   |  4----|--7
    // |   | /     | /
    // |   0-------3
    // |
    //  -----------> X
    // 0 - min
    // 6 - max
    // Interpolation weights are in order: 0, 1, 2, 3, 4, 5, 6, 7

    vec3 extents = pMax - pMin;
    float divisorInv = 1.0 / (extents.x * extents.y * extents.z);

    vec8 weights;
    weights.value0 = (pMax.x - p.x) * (pMax.y - p.y) * (pMax.z - p.z) * divisorInv;
    weights.value1 = (pMax.x - p.x) * (p.y - pMin.y) * (pMax.z - p.z) * divisorInv;
    weights.value2 = (p.x - pMin.x) * (p.y - pMin.y) * (pMax.z - p.z) * divisorInv;
    weights.value3 = (p.x - pMin.x) * (pMax.y - p.y) * (pMax.z - p.z) * divisorInv;
    weights.value4 = (pMax.x - p.x) * (pMax.y - p.y) * (p.z - pMin.z) * divisorInv;
    weights.value5 = (pMax.x - p.x) * (p.y - pMin.y) * (p.z - pMin.z) * divisorInv;
    weights.value6 = (p.x - pMin.x) * (p.y - pMin.y) * (p.z - pMin.z) * divisorInv;
    weights.value7 = (p.x - pMin.x) * (pMax.y - p.y) * (p.z - pMin.z) * divisorInv;

    return weights;
}

SH TriLerpSurroundingProbes(vec3 fragNormal, vec3 fragWorldPos) {

    ivec3 gridSize = textureSize(uGridSHMap0, 0);
    ivec3 gridResolution = gridSize - 1;
    vec3 texCoords = (uWorldBoudningBoxTransform * vec4(fragWorldPos, 1.0)).xyz;

    vec3 unnormCoords = texCoords * vec3(gridResolution);
    vec3 minCoords = floor(unnormCoords);
    vec3 maxCoords = ceil(unnormCoords);

    //
    //        5-------6
    //       /|      /|
    // Y    / |     / |
    // ^   1--|----2  |
    // |   |  4----|--7
    // |   | /     | /
    // |   0-------3
    // |
    //  -----------> X
    //

    vec3 cp0 = vec3(minCoords.x, minCoords.y, minCoords.z); vec3 cp1 = vec3(minCoords.x, maxCoords.y, minCoords.z);
    vec3 cp2 = vec3(maxCoords.x, maxCoords.y, minCoords.z); vec3 cp3 = vec3(maxCoords.x, minCoords.y, minCoords.z);
    vec3 cp4 = vec3(minCoords.x, minCoords.y, maxCoords.z); vec3 cp5 = vec3(minCoords.x, maxCoords.y, maxCoords.z);
    vec3 cp6 = vec3(maxCoords.x, maxCoords.y, maxCoords.z); vec3 cp7 = vec3(maxCoords.x, minCoords.y, maxCoords.z);

    SH sh0 = UnpackSH(cp0); SH sh1 = UnpackSH(cp1);
    SH sh2 = UnpackSH(cp2); SH sh3 = UnpackSH(cp3);
    SH sh4 = UnpackSH(cp4); SH sh5 = UnpackSH(cp5);
    SH sh6 = UnpackSH(cp6); SH sh7 = UnpackSH(cp7);

    float probe0OcclusionFactor = ProbeOcclusionFactor(cp0, gridSize, fragNormal, fragWorldPos);
    float probe1OcclusionFactor = ProbeOcclusionFactor(cp1, gridSize, fragNormal, fragWorldPos);
    float probe2OcclusionFactor = ProbeOcclusionFactor(cp2, gridSize, fragNormal, fragWorldPos);
    float probe3OcclusionFactor = ProbeOcclusionFactor(cp3, gridSize, fragNormal, fragWorldPos);
    float probe4OcclusionFactor = ProbeOcclusionFactor(cp4, gridSize, fragNormal, fragWorldPos);
    float probe5OcclusionFactor = ProbeOcclusionFactor(cp5, gridSize, fragNormal, fragWorldPos);
    float probe6OcclusionFactor = ProbeOcclusionFactor(cp6, gridSize, fragNormal, fragWorldPos);
    float probe7OcclusionFactor = ProbeOcclusionFactor(cp7, gridSize, fragNormal, fragWorldPos);

    vec8 weights = TriLerp(minCoords, maxCoords, unnormCoords);

    float excludedWeight = 0.0;

    excludedWeight += weights.value0 * (1.0 - probe0OcclusionFactor);
    weights.value0 *= probe0OcclusionFactor;

    excludedWeight += weights.value1 * (1.0 - probe1OcclusionFactor);
    weights.value1 *= probe1OcclusionFactor;

    excludedWeight += weights.value2 * (1.0 - probe2OcclusionFactor);
    weights.value2 *= probe2OcclusionFactor;

    excludedWeight += weights.value3 * (1.0 - probe3OcclusionFactor);
    weights.value3 *= probe3OcclusionFactor;

    excludedWeight += weights.value4 * (1.0 - probe4OcclusionFactor);
    weights.value4 *= probe4OcclusionFactor;

    excludedWeight += weights.value5 * (1.0 - probe5OcclusionFactor);
    weights.value5 *= probe5OcclusionFactor;

    excludedWeight += weights.value6 * (1.0 - probe6OcclusionFactor);
    weights.value6 *= probe6OcclusionFactor;

    excludedWeight += weights.value7 * (1.0 - probe7OcclusionFactor);
    weights.value7 *= probe7OcclusionFactor;

    float weightScale = 1.0 / (1.0 - excludedWeight);

    weights.value0 *= weightScale; weights.value1 *= weightScale;
    weights.value2 *= weightScale; weights.value3 *= weightScale;
    weights.value4 *= weightScale; weights.value5 *= weightScale;
    weights.value6 *= weightScale; weights.value7 *= weightScale;

    sh0 = ScaleSH(sh0, vec3(weights.value0)); sh1 = ScaleSH(sh1, vec3(weights.value1));
    sh2 = ScaleSH(sh2, vec3(weights.value2)); sh3 = ScaleSH(sh3, vec3(weights.value3));
    sh4 = ScaleSH(sh4, vec3(weights.value4)); sh5 = ScaleSH(sh5, vec3(weights.value5));
    sh6 = ScaleSH(sh6, vec3(weights.value6)); sh7 = ScaleSH(sh7, vec3(weights.value7));

    SH result = SumSH(sh0, sh1, sh2, sh3, sh4, sh5, sh6, sh7);

    return result;
}

float SHRadiance(SH sh, vec3 direction, int component) {
    int c = component;

    const float Y00 = 0.28209479177387814347; // 1 / (2*sqrt(pi))
    const float Y11 = -0.48860251190291992159; // sqrt(3 /(4pi))
    const float Y10 = 0.48860251190291992159;
    const float Y1_1 = -0.48860251190291992159;
    const float Y21 = -1.09254843059207907054; // 1 / (2*sqrt(pi))
    const float Y2_1 = -1.09254843059207907054;
    const float Y2_2 = 1.09254843059207907054;
    const float Y20 = 0.31539156525252000603; // 1/4 * sqrt(5/pi)
    const float Y22 = 0.54627421529603953527; // 1/4 * sqrt(15/pi)

    float result = 0.0;

    result += sh.L00[c] * Y00;

    result += sh.L1_1[c] * Y1_1 * direction.y;
    result += sh.L10[c] * Y10 * direction.z;
    result += sh.L11[c] * Y11 * direction.x;

    result += sh.L2_2[c] * Y2_2 * (direction.x * direction.y);
    result += sh.L2_1[c] * Y2_1 * (direction.y * direction.z);
    result += sh.L21[c] * Y21 * (direction.x * direction.z);
    result += sh.L20[c] * Y20 * (3.0f * direction.z * direction.z - 1.0f);
    result += sh.L22[c] * Y22 * (direction.x * direction.x - direction.y * direction.y);

    return result;
}

vec3 EvaluateSphericalHarmonics(vec3 direction, vec3 fragWorldPos) {
    SH sh = TriLerpSurroundingProbes(direction, fragWorldPos);
    vec3 color = vec3(SHRadiance(sh, direction, 0), SHRadiance(sh, direction, 1), SHRadiance(sh, direction, 2));
    return color;
}

////////////////////////////////////////////////////////////
//////////////////// Lighting equation /////////////////////
////////////////////////////////////////////////////////////

// Functions

// The Fresnel equation describes the ratio of surface reflection at different surface angles.
// (This is an approximation of Fresnels' equation, called Fresnel-Schlick)
// Describes the ratio of light that gets reflected over the light that gets refracted,
// which varies over the angle we're looking at a surface.
//
vec3 FresnelSchlick(vec3 V, vec3 H, vec3 albedo, float metallic) {
    // F0 - base reflectivity of a surface, a.k.a. surface reflection at zero incidence
    // or how much the surface reflects if looking directly at the surface
    vec3 F0 = vec3(0.04); // 0.04 is a commonly used constant for dielectrics
    F0 = mix(F0, albedo, metallic);
    
    float cosTheta = max(dot(H, V), 0.0);
    
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

// Trowbridge-Reitz GGX normal distribution function
// Statistically approximates the ratio of microfacets aligned to some (halfway) vector h
//
float NormalDistributionGGX(vec3 N, vec3 H, float roughness) {
    float a2        = roughness * roughness;
    float NdotH     = max(dot(N, H), 0.0);
    float NdotH2    = NdotH * NdotH;
    
    float nom       = a2;
    float denom     = (NdotH2 * (a2 - 1.0) + 1.0);
    denom           = PI * denom * denom;
    
    return nom / denom;
}

// Statistically approximates the ratio of microfacets that overshadow each other causing light rays to lose their energy in the process.
// Combination of the GGX and Schlick-Beckmann approximation known as Schlick-GGX.
//
float GeometrySchlickGGX(float NdotV, float roughness) {
    float a         = (roughness + 1.0);
    
    // Here k is a remapping of roughness based on whether we're using the geometry function for either direct lighting or IBL lighting
    float Kdirect   = (a * a) / 8.0;
    float nom       = NdotV;
    float denom     = NdotV * (1.0 - Kdirect) + Kdirect;
    
    return nom / denom;
}

// To effectively approximate the geometry we need to take account of both the view direction (geometry obstruction) and the light direction vector (geometry shadowing).
// We can take both into account using Smith's method:
float GeometrySmith(float NdotL, float NdotV, float roughness) {
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    
    return ggx1 * ggx2;
}

vec3 CookTorranceBRDF(vec3 N, vec3 V, vec3 H, vec3 L, float roughness, vec3 albedo, float metallic, float ao, vec3 radiance, vec3 indirectRadiance, float shadow) {
    float NdotL     = max(dot(N, L), 0.0);
    float NdotV     = max(dot(N, V), 0.0);
    
    float NDF       = NormalDistributionGGX(N, H, roughness);
    float G         = GeometrySmith(NdotL, NdotV, roughness);
    vec3 F          = FresnelSchlick(V, H, albedo, metallic);

    vec3 nom        = NDF * G * F;
    float denom     = 4.0 * NdotL * NdotV + 0.001; // Add 0.001 to prevent division by 0
    vec3 specular   = nom / denom;
    
    vec3 Ks         = F;                // Specular (reflected) portion
    vec3 Kd         = vec3(1.0) - Ks;   // Diffuse (refracted) portion
    
    Kd              *= 1.0 - metallic;  // This will turn diffuse component of metallic surfaces to 0

    // Radiance of analytical light with shadow applied and Normal to Light vectors angle accounted
    vec3 shadowedDirectRadiance = radiance * shadow * NdotL;

    // Lambertian diffuse component with indirect light applied
    vec3 diffuse    = Kd * (albedo /*/ PI*/) * (shadowedDirectRadiance + (indirectRadiance * ao));

    // Specular component is not affected by indirect light (probably will by a reflection probe later)
    specular        *= shadowedDirectRadiance;

    return diffuse + specular;
}

//vec3 IBL(vec3 N, vec3 V, vec3 H, vec3 albedo, float roughness, float metallic) {
//    vec3 R = reflect(-V, N);
//    vec3 diffuseIrradiance  = texture(uDiffuseIrradianceMap, R).rgb;
//    vec3 specularIrradiance = textureLod(uSpecularIrradianceMap, R, roughness * (uSpecularIrradianceMapLOD - 1)).rgb;
//    vec3 F                  = FresnelSchlick(V, H, albedo, metallic);
//    vec2 envBRDF            = texture(uBRDFIntegrationMap, vec2(max(dot(N, V), 0.0), roughness)).rg;
//    
//    vec3 Ks         = F;                // Specular (reflected) portion
//    vec3 Kd         = vec3(1.0) - Ks;   // Diffuse (refracted) portion
//    Kd              *= 1.0 - metallic;  // This will turn diffuse component of metallic surfaces to 0
//    
//    vec3 diffuse    = diffuseIrradiance * albedo;
//    vec3 specular   = (F * envBRDF.x + envBRDF.y) * specularIrradiance;
//
//    return diffuse + specular;
//}

////////////////////////////////////////////////////////////
//////////// Radiance of different light types /////////////
////////////////////////////////////////////////////////////

vec3 PointLightRadiance(vec3 N, vec3 fragWorldPosition) {
    vec3 Wi                 = normalize(uPointLight.position - fragWorldPosition);     // To light vector
    float distance          = length(Wi);                                           // Distance from fragment to light
    float attenuation       = 1.0 / (distance * distance);                          // How much enegry has light lost at current distance
    
    return uPointLight.radiantFlux * attenuation;
}

vec3 DirectionalLightRadiance() {
    return uDirectionalLight.radiantFlux;
}

////////////////////////////////////////////////////////////
///////////////////////// Shadows //////////////////////////
////////////////////////////////////////////////////////////

int ShadowCascadeIndex(vec3 worldPosition)
{
    vec4 posInCSMSplitSpace = uCSMSplitSpaceMat * vec4(worldPosition, 1.0);

    vec3 projCoords = posInCSMSplitSpace.xyz / posInCSMSplitSpace.w;
    // No need to transform to [0,1] range,
    // because splits passed from client are in [-1; 1]

    float fragDepth = projCoords.z;

    for (int i = 0; i < kMaxCascades; ++i) {
        if (fragDepth < uDepthSplits[i]) {
            return i;
        }
    }

    return 0;
}

float ExponentialShadow(vec3 worldPosition) {
    int cascade = ShadowCascadeIndex(worldPosition);
    mat4 relevantLightMatrix = uLightSpaceMatrices[cascade];
    vec4 positionInLightSpace = relevantLightMatrix * vec4(worldPosition, 1.0);

    vec3 projCoords = positionInLightSpace.xyz / positionInLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

    // Linear Z a.k.a disntance from light
    // Distance to the fragment from a near clip plane of directional light's frustum
    float linearZ = positionInLightSpace.z;

    vec4 occluders = texture(uExponentialShadowMap, projCoords.xy);

    float occluder = occluders[cascade];
    float receiver = exp(-uESMFactor * linearZ);
    float visibility = clamp(occluder * receiver, 0.0, 1.0);

    return visibility;
}

////////////////////////////////////////////////////////////
///////////////////////// Reflections  /////////////////////
////////////////////////////////////////////////////////////

//const float step = 0.1;
//const float minRayStep = 0.1;
//const float maxSteps = 30;
//const int numBinarySearchSteps = 5;
//const float reflectionSpecularFalloffExponent = 3.0;
//
//vec3 Hash(vec3 a) {
//    vec3 scale = vec3(0.8);
//    float K = 19.19;
//
//    a = fract(a * scale);
//    a += dot(a, a.yxz + K);
//
//    return fract((a.xxy + a.yxx)*a.zyx);
//}

//vec4 RayMarch(vec3 dir, vec3 worldPos, inout vec3 hitCoord, out float dDepth) {
//
//    dir *= step;
//
//    float depth;
//    int steps;
//    vec4 projectedCoord;
//
//    for(int i = 0; i < maxSteps; i++) {
//
//        hitCoord += dir;
//
//        // Transform our 3D vector into a 2D screen coordinate to detch the depth buffer and check for a collision
//        projectedCoord = projection * vec4(hitCoord, 1.0);
//        projectedCoord.xy /= projectedCoord.w;
//        projectedCoord.xy = projectedCoord.xy * 0.5 + 0.5;
//
//        depth = textureLod(gPosition, projectedCoord.xy, 2).z;
//        if(depth > 1000.0)
//            continue;
//
//        dDepth = hitCoord.z - depth;
//
//        if((dir.z - dDepth) < 1.2) {
//            if(dDepth <= 0.0) {
//                vec4 Result;
//                Result = vec4(BinarySearch(dir, hitCoord, dDepth), 1.0);
//
//                return Result;
//            }
//        }
//
//        steps++;
//    }
//
//    return vec4(projectedCoord.xy, depth, 0.0);
//}

void ScreenSpaceReflection(vec3 N, vec3 worldPosition) {
    vec3 viewNormal = (uCameraViewMat * vec4(N, 0.0)).xyz;
    vec3 viewPosition = (uCameraViewMat * vec4(worldPosition, 1.0)).xyz;

    vec3 reflected = normalize(reflect(normalize(viewPosition), normalize(viewNormal)));
    vec3 hitPosition = viewPosition;

    float depth = 0.0;


}

vec3 ReinhardToneMap(vec3 color) {
    return color / (color + vec3(1.0));
}

vec3 GammaCorrect(vec3 color) {
    return pow(color, vec3(1.0 / 2.2));
}

////////////////////////////////////////////////////////////
////////////////////////// Main ////////////////////////////
////////////////////////////////////////////////////////////

void main() {
    vec4 albedoAndRoughness = texture(uGBuffer, vec3(vTexCoords, 0));
    vec4 normalAndMetalness = texture(uGBuffer, vec3(vTexCoords, 1));
    vec4 worldPositionAndAO = texture(uGBuffer, vec3(vTexCoords, 2));

    vec3 worldPosition = worldPositionAndAO.rgb;

    float roughness         = albedoAndRoughness.a;
    
    // Based on observations by Disney and adopted by Epic Games
    // the lighting looks more correct squaring the roughness
    // in both the geometry and normal distribution function.
    float roughness2        = roughness * roughness;
    
    float metallic          = normalAndMetalness.a;
    float ao                = worldPositionAndAO.a;
    vec3 albedo             = albedoAndRoughness.rgb;
    vec3 N                  = normalAndMetalness.rgb;
    vec3 V                  = normalize(uCameraPosition - worldPosition);
    vec3 L                  = vec3(0.0);
    vec3 H                  = normalize(L + V);
    vec3 radiance           = vec3(0.0);
    float shadow            = 0.0;

    if (!areMaterialsEnabled()) {
        albedo = vec3(1.0);
        ao = 1.0;
        roughness = 1.0;
        roughness2 = 1.0;
        metallic = 0.0;
    }

    // Analytical lighting
    
    if (uLightType == kLightTypeDirectional) {
        radiance    = DirectionalLightRadiance();
        L           = -normalize(uDirectionalLight.direction);
        shadow      = ExponentialShadow(worldPosition);
    }
    else if (uLightType == kLightTypePoint) {
        radiance    = PointLightRadiance(N, worldPosition);
        L           = normalize(uPointLight.position - worldPosition);
    }
    else if (uLightType == kLightTypeSpot) {
        // Nothing to do here... yet
    }

    vec3 indirectRadiance = EvaluateSphericalHarmonics(N, worldPosition);
    indirectRadiance = RGB_From_YCoCg(indirectRadiance);
    indirectRadiance *= isGlobalIlluminationEnabled() ? 1.0 : 0.0;

//    shadow = 1.0;
    vec3 specularAndDiffuse = CookTorranceBRDF(N, V, H, L, roughness2, albedo, metallic, ao, radiance, indirectRadiance, shadow);

    specularAndDiffuse = ReinhardToneMap(specularAndDiffuse);
    specularAndDiffuse = GammaCorrect(specularAndDiffuse);

    oBaseOutput = vec4(specularAndDiffuse, 1.0);

    float luminocity = 0.2126 * specularAndDiffuse.r + 0.7152 * specularAndDiffuse.g + 0.0722 * specularAndDiffuse.b;
    oBrightOutput = luminocity > 1.0 ? oBaseOutput : vec4(0.0, 0.0, 0.0, 1.0);

    /////// DEBUG

//    vec3 viewNormal = (uCameraViewMat * vec4(N, 0.0)).xyz;
//    vec3 viewPosition = (uCameraViewMat * vec4(worldPosition, 1.0)).xyz;
//
//    vec3 reflected = normalize(reflect(normalize(viewPosition), normalize(viewNormal)));
//
//    oBaseOutput = vec4(reflected, 1.0);
}
