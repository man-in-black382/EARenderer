#version 400 core

#include "Packing.glsl"
#include "SphericalHarmonics.glsl"
#include "ColorSpace.glsl"

// Constants

const float PI = 3.1415926535897932384626433832795;
const int kMaxCascades = 4;

const int kLightTypeDirectional = 0;
const int kLightTypePoint       = 1;
const int kLightTypeSpot        = 2;

const int kGBufferIndexPosition = 0;
const int kGBufferIndexNormal   = 1;
const int kGBufferIndexAlbedo   = 2;

// Output

out float oLuminance;

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

uniform DirectionalLight uDirectionalLight;
uniform PointLight uPointLight;
uniform Spotlight uSpotlight;
uniform sampler2DArray uSurfelsGBuffer;

uniform int uLightType;
uniform bool uEnableMultibounce;

// Shadow mapping
uniform mat4 uCSMSplitSpaceMat;
uniform mat4 uLightSpaceMatrices[kMaxCascades];
uniform int uDepthSplitsAxis;
uniform float uDepthSplits[kMaxCascades];
uniform int uNumberOfCascades;
uniform float uESMFactor;
uniform sampler2D uDirectionalShadowMap;
uniform samplerCubeArray uOmnidirectionalShadowMaps;
uniform int uOmnidirectionalShadowMapIndex;

// Spherical harmonics
uniform usampler3D uGridSHMap0;
uniform usampler3D uGridSHMap1;
uniform usampler3D uGridSHMap2;
uniform usampler3D uGridSHMap3;

uniform samplerBuffer uProbePositions;
uniform mat4 uWorldBoudningBoxTransform;

////////////////////////////////////////////////////////////
/////////////////// Spherical harmonics ////////////////////
////////////////////////////////////////////////////////////

struct vec8 {
    float value0; float value1; float value2; float value3;
    float value4; float value5; float value6; float value7;
};

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

float ProbeOcclusionFactor(vec3 probeGridPos, ivec3 gridSize, vec3 fragNorm, vec3 worldPosition) {
    // [x + WIDTH * (y + HEIGHT * z)]
    int gridWidth = gridSize.x;
    int gridHeight = gridSize.y;
    int probeCoord1D = int(probeGridPos.x) + gridWidth * (int(probeGridPos.y) + gridHeight * int(probeGridPos.z));

    vec3 probePosition = texelFetch(uProbePositions, probeCoord1D).xyz;

    vec3 fragToProbeVector = normalize(probePosition - worldPosition);
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

SH TriLerpSurroundingProbes(vec3 fragNormal, vec3 worldPosition) {

    ivec3 gridSize = textureSize(uGridSHMap0, 0);
    ivec3 gridResolution = gridSize - 1;
    vec3 texCoords = (uWorldBoudningBoxTransform * vec4(worldPosition, 1.0)).xyz;

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

    SH sh0 = UnpackSH_333(cp0); SH sh1 = UnpackSH_333(cp1);
    SH sh2 = UnpackSH_333(cp2); SH sh3 = UnpackSH_333(cp3);
    SH sh4 = UnpackSH_333(cp4); SH sh5 = UnpackSH_333(cp5);
    SH sh6 = UnpackSH_333(cp6); SH sh7 = UnpackSH_333(cp7);

    float probe0OcclusionFactor = ProbeOcclusionFactor(cp0, gridSize, fragNormal, worldPosition);
    float probe1OcclusionFactor = ProbeOcclusionFactor(cp1, gridSize, fragNormal, worldPosition);
    float probe2OcclusionFactor = ProbeOcclusionFactor(cp2, gridSize, fragNormal, worldPosition);
    float probe3OcclusionFactor = ProbeOcclusionFactor(cp3, gridSize, fragNormal, worldPosition);
    float probe4OcclusionFactor = ProbeOcclusionFactor(cp4, gridSize, fragNormal, worldPosition);
    float probe5OcclusionFactor = ProbeOcclusionFactor(cp5, gridSize, fragNormal, worldPosition);
    float probe6OcclusionFactor = ProbeOcclusionFactor(cp6, gridSize, fragNormal, worldPosition);
    float probe7OcclusionFactor = ProbeOcclusionFactor(cp7, gridSize, fragNormal, worldPosition);

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

vec3 EvaluateSphericalHarmonics(vec3 direction, vec3 worldPosition) {
    SH sh = TriLerpSurroundingProbes(direction, worldPosition);
    return vec3(EvaluateSH(sh, direction, 0), EvaluateSH(sh, direction, 1), EvaluateSH(sh, direction, 2));
}

////////////////////////////////////////////////////////////
//////////// Radiance of different light types /////////////
////////////////////////////////////////////////////////////

vec3 PointLightRadiance(vec3 N, vec3 position) {
    vec3 Wi                 = uPointLight.position - position;  // To light vector
    float distance          = length(Wi);                       // Distance from fragment to light
    float attenuation       = 1.0 / (distance * distance);      // How much enegry has light lost at current distance

    return uPointLight.radiantFlux * attenuation;
}

vec3 DirectionalLightRadiance() {
    return uDirectionalLight.radiantFlux;
}

////////////////////////////////////////////////////////////
///////////////////////// Shadows //////////////////////////
////////////////////////////////////////////////////////////

int ShadowCascadeIndex(vec3 worldPosition) {

    vec4 posInCSMSplitSpace = uCSMSplitSpaceMat * vec4(worldPosition, 1.0);

    vec3 projCoords = posInCSMSplitSpace.xyz / posInCSMSplitSpace.w;
    // No need to transform to [0,1] range,
    // because splits passed from client are in [-1; 1]

    float locationOnSplitAxis = projCoords[uDepthSplitsAxis];

    for (int i = 0; i < kMaxCascades; ++i) {
        if (locationOnSplitAxis < uDepthSplits[i]) {
            return i;
        }
    }
}

float DirectionalExponentialShadow(vec3 worldPosition) {
    int cascade = ShadowCascadeIndex(worldPosition);
    mat4 relevantLightMatrix = uLightSpaceMatrices[cascade];
    vec4 positionInLightSpace = relevantLightMatrix * vec4(worldPosition, 1.0);

    vec3 projCoords = positionInLightSpace.xyz / positionInLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

    // Linear Z a.k.a disntance from light
    // Distance to the fragment from a near clip plane of directional light's frustum
    float linearZ = positionInLightSpace.z * 0.5 + 0.5; // Transform to [0; 1] range

    // Explicitly reading from 0 LOD because shadow map comes from a postprocess texture pool
    // and is a subject to mipmapping
    vec4 occluders = textureLod(uDirectionalShadowMap, projCoords.xy, 0);

    float occluder = occluders[cascade];
    float receiver = exp(-uESMFactor * linearZ);
    float visibility = clamp(occluder * receiver, 0.0, 1.0);

    return visibility;
}

////////////////////////////////////////////////////////////
////////////////////////// Main ////////////////////////////
////////////////////////////////////////////////////////////

void main() {
    vec3 position  = texelFetch(uSurfelsGBuffer, ivec3(ivec2(gl_FragCoord.xy), int(kGBufferIndexPosition)), 0).rgb;
    vec3 N         = texelFetch(uSurfelsGBuffer, ivec3(ivec2(gl_FragCoord.xy), int(kGBufferIndexNormal)), 0).rgb;
    vec3 L         = vec3(0.0);
    vec3 radiance  = vec3(0.0);
    float shadow   = 0.0;

    // Analytical lighting

    if (uLightType == kLightTypeDirectional) {
        radiance    = DirectionalLightRadiance();
        L           = -normalize(uDirectionalLight.direction);
        shadow      = DirectionalExponentialShadow(position);
    } else if (uLightType == kLightTypePoint) {
        radiance    = PointLightRadiance(N, position);
        L           = normalize(uPointLight.position - position);
    } else if (uLightType == kLightTypeSpot) {
        // Nothing to do here... yet
    }

    float NdotL = max(dot(N, L), 0.0);

    // Surfel's color (albedo) is not needed here because it's already incoded
    // in the precomputed spherical harmonics.
    vec3 diffuseRadiance = radiance * NdotL;
    
    vec3 indirectRadiance = uEnableMultibounce ? EvaluateSphericalHarmonics(N, position) : vec3(0.0);
    indirectRadiance = RGB_From_YCoCg(indirectRadiance);
    indirectRadiance = max(vec3(0.0), indirectRadiance);

    // Apply shadow factor
    diffuseRadiance *= shadow;

    vec3 finalColor = diffuseRadiance;// + indirectRadiance;

    oLuminance = LuminanceFromRGB(finalColor);
}
