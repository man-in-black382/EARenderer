#version 400 core

#include "Packing.glsl"
#include "ColorSpace.glsl"
#include "SphericalHarmonics.glsl"

// Constants

const float kDenormalizationFactor = 1000.0;
const float PI = 3.1415926535897932384626433832795;

const int kSHCompression333 = 0;
const int kSHCompression322 = 1;
const int kSHCompression311 = 2;

// Output

layout(location = 0) out vec4 oBaseOutput;
layout(location = 1) out vec4 oBrightOutput;

// Input

in vec2 vTexCoords;

// Uniforms
struct vec8 {
    float value0; float value1; float value2; float value3;
    float value4; float value5; float value6; float value7;
};

uniform usampler2D uGBufferAlbedoRoughnessMetalnessAONormal;
uniform sampler2D uGBufferHiZBuffer;

uniform vec3 uCameraPosition;
uniform mat4 uCameraViewInverse;
uniform mat4 uCameraProjectionInverse;
uniform vec2 uCameraNearFarPlanes;
uniform mat4 uWorldBoudningBoxTransform;

uniform uint uSettingsBitmask;
uniform float uParallaxMappingStrength;
uniform int uSHCompressionType;

// Shperical harmonics

uniform usampler3D uGridSHMap0;
uniform usampler3D uGridSHMap1;
uniform usampler3D uGridSHMap2;
uniform usampler3D uGridSHMap3;

uniform samplerBuffer uProbePositions;

uniform sampler2D uLightBuffer;
uniform sampler2D uReflections;

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
    return UnpackSH_333(texCoords);
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

    SH result = Sum8SH(sh0, sh1, sh2, sh3, sh4, sh5, sh6, sh7);

    return result;
}

vec3 EvaluateSphericalHarmonics(vec3 direction, vec3 fragWorldPos) {
    SH sh = TriLerpSurroundingProbes(direction, fragWorldPos);
    vec3 color = vec3(EvaluateSH(sh, direction, 0), EvaluateSH(sh, direction, 1), EvaluateSH(sh, direction, 2));
    return color;
}

////////////////////////////////////////////////////////////
//////////////////// Lighting equation /////////////////////
////////////////////////////////////////////////////////////

// The Fresnel equation describes the ratio of surface reflection at different surface angles.
// (This is an approximation of Fresnels' equation, called Fresnel-Schlick)
// Describes the ratio of light that gets reflected over the light that gets refracted,
// which varies over the angle we're looking at a surface.
//
vec3 FresnelSchlick(vec3 V, vec3 H, vec3 albedo, float metallic) {
    // F0 - base reflectivity of a surface, a.k.a. surface reflection at zero incidence
    // or how much the surface reflects if looking directly at it
    vec3 F0 = vec3(0.04); // 0.04 is a commonly used constant for dielectrics
    F0 = mix(F0, albedo, metallic);
    
    float cosTheta = max(dot(H, V), 0.0);
    
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

////////////////////////////////////////////////////////////
//////////////////////// G Buffer //////////////////////////
////////////////////////////////////////////////////////////

struct GBuffer {
    vec3 albedo;
    vec3 normal;
    float roughness;
    float metalness;
    float AO;
};

vec4 Decode8888(uint encoded) {
    vec4 decoded;
    decoded.x = (0xFF000000u & encoded) >> 24;
    decoded.y = (0x00FF0000u & encoded) >> 16;
    decoded.z = (0x0000FF00u & encoded) >> 8;
    decoded.w = (0x000000FFu & encoded);
    decoded /= 255.0;
    return decoded;
}

// GBuffer packing scheme
//
// | Albedo R | Albedo G | Albedo B | Roughness | Metalness |    AO    |        Normal Z      |
// |          |          |          |           |           |          |                      |
// |  1 Byte  |  1 Byte  |  1 Byte  |   1 Byte  |  1 Byte   |  1 Byte  |        2 Bytes       |
// |__________|__________|__________|___________|___________|__________|______________________|
// |______First component of output UVEC3_______|_______Second component of output UVEC3______|
//
//
// |        Normal X      |        Normal Y      |
// |                      |                      |
// |        2 Bytes       |        2 Bytes       |
// |______________________|______________________|
// |________Third component of output UVEC3______|

GBuffer DecodeGBuffer() {
    GBuffer gBuffer;

    uvec3 albedoRoughnessMetalnessAONormal = texture(uGBufferAlbedoRoughnessMetalnessAONormal, vTexCoords.st).xyz;
    vec4 albedoRoughness = Decode8888(albedoRoughnessMetalnessAONormal.x);
    uint metalnessAONormalZ = albedoRoughnessMetalnessAONormal.y;
    vec2 metalnessAO = Decode8888(metalnessAONormalZ).xy;
    float normalZ = UnpackSnorm2x16(metalnessAONormalZ, 1.0).y;
    vec2 normalXY = UnpackSnorm2x16(albedoRoughnessMetalnessAONormal.z, 1.0);

    gBuffer.albedo    = albedoRoughness.rgb;
    gBuffer.normal    = vec3(normalXY, normalZ);
    gBuffer.roughness = albedoRoughness.a;
    gBuffer.metalness = metalnessAO.r;
    gBuffer.AO        = metalnessAO.g;

    return gBuffer;
}

// Input is linear [0; 1] depth value
// Output is [-1; 1] non-linear depth value ready to be used in WP reconstruction
float NDCHyperbolicDepthFromLinearDepth(float linearDepth) {
    float zNear = uCameraNearFarPlanes.x;
    float zFar = uCameraNearFarPlanes.y;
    float nonLinearDepth = (zFar + zNear - 2.0 * zNear * zFar / linearDepth) / (zFar - zNear);
    return nonLinearDepth;
}

vec3 ReconstructWorldPosition() {
    float depth = texture(uGBufferHiZBuffer, vTexCoords).r;

    // Depth range in NDC is [-1; 1]
    // Default value for glDepthRange is [-1; 1]
    // OpenGL uses values from glDepthRange to transform depth to [0; 1] range during viewport transformation
    // To reconstruct world position using inverse camera matrices we need depth in [-1; 1] range again
    float z = depth * 2.0 - 1.0;
    vec2 xy = vTexCoords * 2.0 - 1.0;

    vec4 clipSpacePosition = vec4(xy, z, 1.0);
    vec4 viewSpacePosition = uCameraProjectionInverse * clipSpacePosition;

    // Perspective division
    viewSpacePosition /= viewSpacePosition.w;

    vec4 worldSpacePosition = uCameraViewInverse * viewSpacePosition;

    return worldSpacePosition.xyz;
}

////////////////////////////////////////////////////////////
////////////////////////// Main ////////////////////////////
////////////////////////////////////////////////////////////

void main() {
    GBuffer gBuffer     = DecodeGBuffer();

    vec3 worldPosition  = ReconstructWorldPosition();

    float ao            = gBuffer.AO;
    float metalness     = gBuffer.metalness;
    vec3 albedo         = gBuffer.albedo;
    vec3 N              = gBuffer.normal;
    vec3 V              = normalize(uCameraPosition - worldPosition);

    // For analytical lights we compute Half-vector used in Fresnel calculations
    // using to-light and to-viewer vectors, but for specular reflections
    // to-light vector is replaced by the fragment's normal
    vec3 H              = normalize(N + V);

    vec3 Ks = FresnelSchlick(V, H, albedo, metalness); // Reflected portion
    vec3 Kd = 1.0 - Ks; // Refracted portion

    vec3 indirectRadiance = EvaluateSphericalHarmonics(N, worldPosition);
    indirectRadiance = RGB_From_YCoCg(indirectRadiance);
//    indirectRadiance *= isGlobalIlluminationEnabled() ? 1.0 : 0.0;
    indirectRadiance *= Kd;

    // Remember that color values in light buffer were normalized by a normalization factor
    vec3 lightBufferColor = texture(uLightBuffer, vTexCoords).rgb * kDenormalizationFactor;

    vec3 SSR = texture(uReflections, vTexCoords).rgb;
    SSR *= Ks;

    vec3 fragmentColor = lightBufferColor + SSR + (indirectRadiance * albedo * ao);

    vec3 factors = vec3(0.2126, 0.7152, 0.0722);
    float luminocity = dot(fragmentColor, factors);

    oBaseOutput = vec4(fragmentColor, 1.0);
    oBrightOutput = luminocity > 1.0 ? oBaseOutput : vec4(0.0, 0.0, 0.0, 1.0);
}
