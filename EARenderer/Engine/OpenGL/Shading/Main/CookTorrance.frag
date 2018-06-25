#version 400 core

// Constants

const float PI = 3.1415926535897932384626433832795;
const int kMaxCascades = 4;

const int kLightTypeDirectional = 0;
const int kLightTypePoint       = 1;
const int kLightTypeSpot        = 2;

// Spherical harmonics
const float kC1 = 0.429043;
const float kC2 = 0.511664;
const float kC3 = 0.743125;
const float kC4 = 0.886227;
const float kC5 = 0.247708;

// Output

out vec4 oFragColor;

// Input

in vec3 vTexCoords;
in vec3 vWorldPosition;
in mat3 vTBN;
in vec4 vPosInLightSpace;
//in vec3 vPosInTangentSpace;
//in vec3 vCameraPosInTangentSpace;

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

struct Material {
    sampler2D albedoMap;
    sampler2D normalMap;
    sampler2D metallicMap;
    sampler2D roughnessMap;
    sampler2D AOMap; // Ambient occlusion
    sampler2D displacementMap; // Parallax occlusion displacements
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

uniform vec3 uCameraPosition;
uniform mat4 uWorldBoudningBoxTransform;

uniform DirectionalLight uDirectionalLight;
uniform PointLight uPointLight;
uniform Spotlight uSpotlight;
uniform Material uMaterial;

uniform int uLightType;
uniform uint uSettingsBitmask;
uniform float uParallaxMappingStrength;

// Shadow mapping
uniform float uDepthSplits[kMaxCascades];
uniform int uNumberOfCascades;

uniform float uESMFactor;
uniform float uESMDarkeningFactor;
uniform sampler2D uExponentialShadowMap;

// Shperical harmonics

uniform usampler3D uGridSHMap0;
uniform usampler3D uGridSHMap1;
uniform usampler3D uGridSHMap2;
uniform usampler3D uGridSHMap3;

uniform samplerBuffer uProbePositions;

// IBL
//uniform sampler2D uBRDFIntegrationMap;
//uniform samplerCube uSpecularIrradianceMap;
//uniform samplerCube uDiffuseIrradianceMap;
//uniform int uSpecularIrradianceMapLOD;

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

SH UnpackSH_311_HalfPacked(vec3 texCoords) {
    SH sh = ZeroSH();

    ivec3 iTexCoords = ivec3(texCoords);

    uvec4 shMap0Data = texelFetch(uGridSHMap0, iTexCoords, 0);
    uvec4 shMap1Data = texelFetch(uGridSHMap1, iTexCoords, 0);

    float range = uintBitsToFloat(shMap1Data.a);

    vec2 pair0 = UnpackSnorm2x16(shMap0Data.r, range); vec2 pair1 = UnpackSnorm2x16(shMap0Data.g, range);
    vec2 pair2 = UnpackSnorm2x16(shMap0Data.b, range); vec2 pair3 = UnpackSnorm2x16(shMap0Data.a, range);
    vec2 pair4 = UnpackSnorm2x16(shMap1Data.r, range); vec2 pair5 = UnpackSnorm2x16(shMap1Data.g, range);

    // Y
    sh.L00.r  = pair0.x;  sh.L11.r  = pair0.y;  sh.L10.r  = pair1.x;
    sh.L1_1.r = pair1.y;  sh.L21.r  = pair2.x;  sh.L2_1.r = pair2.y;
    sh.L2_2.r = pair3.x;  sh.L20.r  = pair3.y;  sh.L22.r  = pair4.x;

    // Co, Cg
    sh.L00.g  = pair4.y; sh.L00.b  = pair5.x;

    return sh;
}

SH UnpackSH_322_HalfPacked(vec3 texCoords) {
    SH sh = ZeroSH();

    ivec3 iTexCoords = ivec3(texCoords);

    uvec4 shMap0Data = texelFetch(uGridSHMap0, iTexCoords, 0);
    uvec4 shMap1Data = texelFetch(uGridSHMap1, iTexCoords, 0);
    uvec4 shMap2Data = texelFetch(uGridSHMap2, iTexCoords, 0);

    float range = uintBitsToFloat(shMap2Data.a);

    vec2 pair0 = UnpackSnorm2x16(shMap0Data.r, range);
    vec2 pair1 = UnpackSnorm2x16(shMap0Data.g, range);
    vec2 pair2 = UnpackSnorm2x16(shMap0Data.b, range);
    vec2 pair3 = UnpackSnorm2x16(shMap0Data.a, range);
    vec2 pair4 = UnpackSnorm2x16(shMap1Data.r, range);
    vec2 pair5 = UnpackSnorm2x16(shMap1Data.g, range);
    vec2 pair6 = UnpackSnorm2x16(shMap1Data.b, range);
    vec2 pair7 = UnpackSnorm2x16(shMap1Data.a, range);
    vec2 pair8 = UnpackSnorm2x16(shMap2Data.r, range);

    // Y
    sh.L00.r  = pair0.x;  sh.L11.r  = pair0.y;  sh.L10.r  = pair1.x;
    sh.L1_1.r = pair1.y;  sh.L21.r  = pair2.x;  sh.L2_1.r = pair2.y;
    sh.L2_2.r = pair3.x;  sh.L20.r  = pair3.y;  sh.L22.r  = pair4.x;

    // Co
    sh.L00.g  = pair4.y;  sh.L11.g  = pair5.x;  sh.L10.g  = pair5.y; sh.L1_1.g = pair6.x;

    // Cg
    sh.L00.b  = pair6.x;  sh.L11.b  = pair7.x;  sh.L10.b  = pair7.y; sh.L1_1.b = pair8.x;

    return sh;
}

SH UnpackSH_333_HalfPacked(vec3 texCoords) {
    SH sh = ZeroSH();

    ivec3 iTexCoords = ivec3(texCoords);

    uvec4 shMap0Data = texelFetch(uGridSHMap0, iTexCoords, 0);
    uvec4 shMap1Data = texelFetch(uGridSHMap1, iTexCoords, 0);
    uvec4 shMap2Data = texelFetch(uGridSHMap2, iTexCoords, 0);
    uvec4 shMap3Data = texelFetch(uGridSHMap3, iTexCoords, 0);

    float range = uintBitsToFloat(shMap3Data.a);

    vec2 pair0 = UnpackSnorm2x16(shMap0Data.r, range);  vec2 pair1 = UnpackSnorm2x16(shMap0Data.g, range);
    vec2 pair2 = UnpackSnorm2x16(shMap0Data.b, range);  vec2 pair3 = UnpackSnorm2x16(shMap0Data.a, range);
    vec2 pair4 = UnpackSnorm2x16(shMap1Data.r, range);  vec2 pair5 = UnpackSnorm2x16(shMap1Data.g, range);
    vec2 pair6 = UnpackSnorm2x16(shMap1Data.b, range);  vec2 pair7 = UnpackSnorm2x16(shMap1Data.a, range);
    vec2 pair8 = UnpackSnorm2x16(shMap2Data.r, range);  vec2 pair9 = UnpackSnorm2x16(shMap2Data.g, range);
    vec2 pair10 = UnpackSnorm2x16(shMap2Data.b, range); vec2 pair11 = UnpackSnorm2x16(shMap2Data.a, range);
    vec2 pair12 = UnpackSnorm2x16(shMap3Data.r, range); vec2 pair13 = UnpackSnorm2x16(shMap3Data.g, range);

    // Y | R
    sh.L00.r  = pair0.x;  sh.L11.r  = pair0.y;  sh.L10.r  = pair1.x;
    sh.L1_1.r = pair1.y;  sh.L21.r  = pair2.x;  sh.L2_1.r = pair2.y;
    sh.L2_2.r = pair3.x;  sh.L20.r  = pair3.y;  sh.L22.r  = pair4.x;

    // Co | G
    sh.L00.g  = pair4.y;  sh.L11.g  = pair5.x;  sh.L10.g  = pair5.y;
    sh.L1_1.g = pair6.x;  sh.L21.g  = pair6.y;  sh.L2_1.g = pair7.x;
    sh.L2_2.g = pair7.y;  sh.L20.g  = pair8.x;  sh.L22.g  = pair8.y;

    // Cg | B
    sh.L00.b  = pair9.x;  sh.L11.b  = pair9.y;  sh.L10.b  = pair10.x;
    sh.L1_1.b = pair10.y;  sh.L21.b  = pair11.x;  sh.L2_1.b = pair11.y;
    sh.L2_2.b = pair12.x;  sh.L20.b  = pair12.y;  sh.L22.b  = pair13.x;

    return sh;
}

float ProbeOcclusionFactor(vec3 probeGridPos, ivec3 gridSize, vec3 fragNorm) {

    // [x + WIDTH * (y + HEIGHT * z)]
    int gridWidth = gridSize.x;
    int gridHeight = gridSize.y;
    int probeCoord1D = int(probeGridPos.x) + gridWidth * (int(probeGridPos.y) + gridHeight * int(probeGridPos.z));

    vec3 probePosition = texelFetch(uProbePositions, probeCoord1D).xyz;

    vec3 fragToProbeVector = normalize(probePosition - vWorldPosition);
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

SH TriLerpSurroundingProbes(vec3 fragNormal) {

    ivec3 gridSize = textureSize(uGridSHMap0, 0);
    ivec3 gridResolution = gridSize - 1;
    vec3 texCoords = (uWorldBoudningBoxTransform * vec4(vWorldPosition, 1.0)).xyz;

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

    SH sh0 = UnpackSH_322_HalfPacked(cp0); SH sh1 = UnpackSH_322_HalfPacked(cp1);
    SH sh2 = UnpackSH_322_HalfPacked(cp2); SH sh3 = UnpackSH_322_HalfPacked(cp3);
    SH sh4 = UnpackSH_322_HalfPacked(cp4); SH sh5 = UnpackSH_322_HalfPacked(cp5);
    SH sh6 = UnpackSH_322_HalfPacked(cp6); SH sh7 = UnpackSH_322_HalfPacked(cp7);

    float probe0OcclusionFactor = ProbeOcclusionFactor(cp0, gridSize, fragNormal); float probe1OcclusionFactor = ProbeOcclusionFactor(cp1, gridSize, fragNormal);
    float probe2OcclusionFactor = ProbeOcclusionFactor(cp2, gridSize, fragNormal); float probe3OcclusionFactor = ProbeOcclusionFactor(cp3, gridSize, fragNormal);
    float probe4OcclusionFactor = ProbeOcclusionFactor(cp4, gridSize, fragNormal); float probe5OcclusionFactor = ProbeOcclusionFactor(cp5, gridSize, fragNormal);
    float probe6OcclusionFactor = ProbeOcclusionFactor(cp6, gridSize, fragNormal); float probe7OcclusionFactor = ProbeOcclusionFactor(cp7, gridSize, fragNormal);

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

    float Y00 = 0.28209479177387814347; // 1 / (2*sqrt(pi))
    float Y11 = -0.48860251190291992159; // sqrt(3 /(4pi))
    float Y10 = 0.48860251190291992159;
    float Y1_1 = -0.48860251190291992159;
    float Y21 = -1.09254843059207907054; // 1 / (2*sqrt(pi))
    float Y2_1 = -1.09254843059207907054;
    float Y2_2 = 1.09254843059207907054;
    float Y20 = 0.31539156525252000603; // 1/4 * sqrt(5/pi)
    float Y22 = 0.54627421529603953527; // 1/4 * sqrt(15/pi)

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

//    int c = component;
//    return  kC1 * sh.L22[c] * (direction.x * direction.x - direction.y * direction.y) +
//            kC3 * sh.L20[c] * (direction.z * direction.z) +
//            kC4 * sh.L00[c] -
//            kC5 * sh.L20[c] +
//            2.0 * kC1 * (sh.L2_2[c] * direction.x * direction.y + sh.L21[c] * direction.x * direction.z + sh.L2_1[c] * direction.y * direction.z) +
//            2.0 * kC2 * (sh.L11[c] * direction.x + sh.L1_1[c] * direction.y + sh.L10[c] * direction.z);
}

vec3 EvaluateSphericalHarmonics(vec3 direction) {
    SH sh = TriLerpSurroundingProbes(direction);
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
    vec3 shadowedDirectRadiance = radiance * (1.0 - shadow) * NdotL;

    // Lambertian diffuse component with indirect light applied
    vec3 diffuse    = Kd * (albedo/* / PI*/) * (shadowedDirectRadiance + (indirectRadiance * ao));

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

vec3 PointLightRadiance(vec3 N) {
    vec3 Wi                 = normalize(uPointLight.position - vWorldPosition);     // To light vector
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

float ExponentialShadow() {
    vec3 projCoords = vPosInLightSpace.xyz / vPosInLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

    // Linear Z a.k.a disntance from light
    // Distance to the fragment from a near clip plane of directional light's frustum
    float linearZ = vPosInLightSpace.z;

    float occluder = texture(uExponentialShadowMap, projCoords.xy).r;
    float receiver = exp(uESMDarkeningFactor - uESMFactor * linearZ);
    float visibility = clamp(occluder * receiver, 0.0, 1.0);

    return 1.0 - visibility;
}

////////////////////////////////////////////////////////////
///////////////////////// Helpers //////////////////////////
////////////////////////////////////////////////////////////

vec3 ReinhardToneMap(vec3 color) {
    return color / (color + vec3(1.0));
}

vec3 GammaCorrect(vec3 color) {
    return pow(color, vec3(1.0 / 2.2));
}

vec3 LinearFromSRGB(vec3 sRGB) {
    return pow(sRGB, vec3(2.2));
}

vec3 FetchAlbedoMap(vec2 texCoords) {
    return LinearFromSRGB(texture(uMaterial.albedoMap, texCoords).rgb);
}

vec3 FetchNormalMap(vec2 texCoords) {
    vec3 normal = texture(uMaterial.normalMap, texCoords).xyz;
    return normalize(vTBN * (normal * 2.0 - 1.0));
}

float FetchMetallicMap(vec2 texCoords) {
    return texture(uMaterial.metallicMap, texCoords).r;
}

float FetchRoughnessMap(vec2 texCoords) {
    return texture(uMaterial.roughnessMap, texCoords).r;
}

float FetchAOMap(vec2 texCoords) {
    return texture(uMaterial.AOMap, texCoords).r;
}

float FetchDisplacementMap() {
    return texture(uMaterial.displacementMap, vTexCoords.st).r;
}

////////////////////////////////////////////////////////////
////////////// Parallax Occlusion Mapping //////////////////
////////////////////////////////////////////////////////////

//vec2 DisplacedTextureCoords() {
//    vec2 texCoords = vTexCoords.st;
//    vec3 viewDir = normalize(vCameraPosInTangentSpace - vPosInTangentSpace);
//
////  number of depth layers
//    const float minLayers = 8;
//    const float maxLayers = 32;
//    float numLayers = mix(maxLayers, minLayers, abs(dot(vec3(0.0, 0.0, 1.0), viewDir)));
//    // calculate the size of each layer
//    float layerDepth = 1.0 / numLayers;
//    // depth of current layer
//    float currentLayerDepth = 0.0;
//    // the amount to shift the texture coordinates per layer (from vector P)
//    vec2 P = viewDir.xy / viewDir.z * uParallaxMappingStrength;
//
//    vec2 deltaTexCoords = P / numLayers;
//
//    // get initial values
//    vec2  currentTexCoords     = texCoords;
//    float currentDepthMapValue = 1.0 - texture(uMaterial.displacementMap, currentTexCoords).r;
//
//    while(currentLayerDepth < currentDepthMapValue) {
//        // shift texture coordinates along direction of P
//        currentTexCoords -= deltaTexCoords;
//        // get depthmap value at current texture coordinates
//        currentDepthMapValue = 1.0 - texture(uMaterial.displacementMap, currentTexCoords).r;
//        // get depth of next layer
//        currentLayerDepth += layerDepth;
//    }
//
//    // get texture coordinates before collision (reverse operations)
//    vec2 prevTexCoords = currentTexCoords + deltaTexCoords;
//
//    // get depth after and before collision for linear interpolation
//    float afterDepth  = currentDepthMapValue - currentLayerDepth;
//    float beforeDepth = (1.0 - texture(uMaterial.displacementMap, prevTexCoords).r) - currentLayerDepth + layerDepth;
//
//    // interpolation of texture coordinates
//    float weight = afterDepth / (afterDepth - beforeDepth);
//    vec2 finalTexCoords = prevTexCoords * weight + currentTexCoords * (1.0 - weight);
//
//    return finalTexCoords;
//}

////////////////////////////////////////////////////////////
////////////////////////// Main ////////////////////////////
////////////////////////////////////////////////////////////

void main() {
    vec2 displacedTexCoords = /*isParallaxMappingEnabled() ? DisplacedTextureCoords() : */vTexCoords.st;

    float roughness         = FetchRoughnessMap(displacedTexCoords);
    
    // Based on observations by Disney and adopted by Epic Games
    // the lighting looks more correct squaring the roughness
    // in both the geometry and normal distribution function.
    float roughness2        = roughness * roughness;
    
    float metallic          = FetchMetallicMap(displacedTexCoords);
    float ao                = FetchAOMap(displacedTexCoords);
    vec3 albedo             = FetchAlbedoMap(displacedTexCoords);
    vec3 N                  = FetchNormalMap(displacedTexCoords);
    vec3 V                  = normalize(uCameraPosition - vWorldPosition);
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
        shadow      = ExponentialShadow();
    }
    else if (uLightType == kLightTypePoint) {
        radiance    = PointLightRadiance(N);
        L           = normalize(uPointLight.position - vWorldPosition);
    }
    else if (uLightType == kLightTypeSpot) {
        // Nothing to do here... yet
    }

    vec3 indirectRadiance = EvaluateSphericalHarmonics(N);
    indirectRadiance = RGB_From_YCoCg(indirectRadiance);
    indirectRadiance *= isGlobalIlluminationEnabled() ? 1.0 : 0.0;

    vec3 specularAndDiffuse = CookTorranceBRDF(N, V, H, L, roughness2, albedo, metallic, ao, radiance, indirectRadiance, shadow);

    // Image based lighting
//    vec3 ambient            = /*IBL(N, V, H, albedo, roughness, metallic)*/vec3(0.01) * ao * albedo;
    vec3 toneMappedColor       = ReinhardToneMap(indirectRadiance);
    vec3 correctColor          = GammaCorrect(toneMappedColor);

    oFragColor = vec4(correctColor, 1.0);
}
