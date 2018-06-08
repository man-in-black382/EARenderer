#version 400 core

// Constants

const float PI = 3.1415926535897932384626433832795;
const int kMaxCascades = 4;

const int kLightTypeDirectional = 0;
const int kLightTypePoint       = 1;
const int kLightTypeSpot        = 2;

const int kGBufferIndexPosition = 0;
const int kGBufferIndexNormal   = 1;
const int kGBufferIndexAlbedo   = 2;
const int kGBufferIndexUV       = 3;

// Spherical harmonics
const float kC1 = 0.429043;
const float kC2 = 0.511664;
const float kC3 = 0.743125;
const float kC4 = 0.886227;
const float kC5 = 0.247708;

// Output

out vec4 oFragColor;

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
uniform sampler2DArray uShadowMapArray;
uniform float uDepthSplits[kMaxCascades];
uniform int uNumberOfCascades;
uniform mat4 uLightSpaceMatrices[kMaxCascades];

// Spherical harmonics
uniform sampler3D uGridSHMap0;
uniform sampler3D uGridSHMap1;
uniform sampler3D uGridSHMap2;
uniform sampler3D uGridSHMap3;
uniform sampler3D uGridSHMap4;
uniform sampler3D uGridSHMap5;
uniform sampler3D uGridSHMap6;

uniform samplerBuffer uProbePositions;
uniform mat4 uWorldBoudningBoxTransform;

////////////////////////////////////////////////////////////
/////////////////// Spherical harmonics ////////////////////
////////////////////////////////////////////////////////////

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

SH ZeroSH() {
    SH result;
    result.L00  = vec3(0.0); result.L1_1 = vec3(0.0); result.L10  = vec3(0.0);
    result.L11  = vec3(0.0); result.L2_2 = vec3(0.0); result.L2_1 = vec3(0.0);
    result.L21  = vec3(0.0); result.L20  = vec3(0.0); result.L22  = vec3(0.0);
    return result;
}

SH ScaleSH(SH sh, vec3 scale) {
    SH result;

    result.L00  = scale * sh.L00;

    result.L1_1 = scale * sh.L1_1;
    result.L10  = scale * sh.L10;
    result.L11  = scale * sh.L11;

    result.L2_2 = scale * sh.L2_2;
    result.L2_1 = scale * sh.L2_1;
    result.L21  = scale * sh.L21;

    result.L20  = scale * sh.L20;

    result.L22  = scale * sh.L22;

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

SH UnpackSH_333(vec3 texCoords) {
    SH sh;

    ivec3 iTexCoords = ivec3(texCoords);

    vec4 shMap0Data = texelFetch(uGridSHMap0, iTexCoords, 0);
    vec4 shMap1Data = texelFetch(uGridSHMap1, iTexCoords, 0);
    vec4 shMap2Data = texelFetch(uGridSHMap2, iTexCoords, 0);
    vec4 shMap3Data = texelFetch(uGridSHMap3, iTexCoords, 0);
    vec4 shMap4Data = texelFetch(uGridSHMap4, iTexCoords, 0);
    vec4 shMap5Data = texelFetch(uGridSHMap5, iTexCoords, 0);
    vec4 shMap6Data = texelFetch(uGridSHMap6, iTexCoords, 0);

    sh.L00  = vec3(shMap0Data.rgb);
    sh.L11  = vec3(shMap0Data.a, shMap1Data.rg);
    sh.L10  = vec3(shMap1Data.ba, shMap2Data.r);
    sh.L1_1 = vec3(shMap2Data.gba);
    sh.L21  = vec3(shMap3Data.rgb);
    sh.L2_1 = vec3(shMap3Data.a, shMap4Data.rg);
    sh.L2_2 = vec3(shMap4Data.ba, shMap5Data.r);
    sh.L20  = vec3(shMap5Data.gba);
    sh.L22  = vec3(shMap6Data.rgb);

    return sh;
}

SH UnpackSH_322(vec3 texCoords) {
    SH sh = ZeroSH();

    ivec3 iTexCoords = ivec3(texCoords);

    vec4 shMap0Data = texelFetch(uGridSHMap0, iTexCoords, 0);
    vec4 shMap1Data = texelFetch(uGridSHMap1, iTexCoords, 0);
    vec4 shMap2Data = texelFetch(uGridSHMap2, iTexCoords, 0);
    vec4 shMap3Data = texelFetch(uGridSHMap3, iTexCoords, 0);
    vec4 shMap4Data = texelFetch(uGridSHMap4, iTexCoords, 0);

    // Y
    sh.L00.r = shMap0Data.r; sh.L11.r = shMap0Data.g; sh.L10.r = shMap0Data.b;
    sh.L1_1.r = shMap0Data.a; sh.L21.r = shMap1Data.r; sh.L2_1.r = shMap1Data.g;
    sh.L2_2.r = shMap1Data.b; sh.L20.r = shMap1Data.a; sh.L22.r = shMap2Data.r;

    // Co
    sh.L00.g = shMap2Data.g; sh.L11.g = shMap2Data.b; sh.L10.g = shMap2Data.a; sh.L1_1.g = shMap3Data.r;

    // Cg
    sh.L00.b = shMap3Data.g; sh.L11.b = shMap3Data.b; sh.L10.b = shMap3Data.a; sh.L1_1.b = shMap4Data.r;

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

    vec3 cp0 = vec3(minCoords.x, minCoords.y, minCoords.z);
    vec3 cp1 = vec3(minCoords.x, maxCoords.y, minCoords.z);
    vec3 cp2 = vec3(maxCoords.x, maxCoords.y, minCoords.z);
    vec3 cp3 = vec3(maxCoords.x, minCoords.y, minCoords.z);
    vec3 cp4 = vec3(minCoords.x, minCoords.y, maxCoords.z);
    vec3 cp5 = vec3(minCoords.x, maxCoords.y, maxCoords.z);
    vec3 cp6 = vec3(maxCoords.x, maxCoords.y, maxCoords.z);
    vec3 cp7 = vec3(maxCoords.x, minCoords.y, maxCoords.z);

    SH sh0 = UnpackSH_322(cp0);
    SH sh1 = UnpackSH_322(cp1);
    SH sh2 = UnpackSH_322(cp2);
    SH sh3 = UnpackSH_322(cp3);
    SH sh4 = UnpackSH_322(cp4);
    SH sh5 = UnpackSH_322(cp5);
    SH sh6 = UnpackSH_322(cp6);
    SH sh7 = UnpackSH_322(cp7);

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

    weights.value0 *= weightScale;
    weights.value1 *= weightScale;
    weights.value2 *= weightScale;
    weights.value3 *= weightScale;
    weights.value4 *= weightScale;
    weights.value5 *= weightScale;
    weights.value6 *= weightScale;
    weights.value7 *= weightScale;

    sh0 = ScaleSH(sh0, vec3(weights.value0));
    sh1 = ScaleSH(sh1, vec3(weights.value1));
    sh2 = ScaleSH(sh2, vec3(weights.value2));
    sh3 = ScaleSH(sh3, vec3(weights.value3));
    sh4 = ScaleSH(sh4, vec3(weights.value4));
    sh5 = ScaleSH(sh5, vec3(weights.value5));
    sh6 = ScaleSH(sh6, vec3(weights.value6));
    sh7 = ScaleSH(sh7, vec3(weights.value7));

    SH result = SumSH(sh0, sh1, sh2, sh3, sh4, sh5, sh6, sh7);

    return result;
}

float SHRadiance(SH sh, vec3 direction, int component) {
    int c = component;
    return  kC1 * sh.L22[c] * (direction.x * direction.x - direction.y * direction.y) +
    kC3 * sh.L20[c] * (direction.z * direction.z) +
    kC4 * sh.L00[c] -
    kC5 * sh.L20[c] +
    2.0 * kC1 * (sh.L2_2[c] * direction.x * direction.y + sh.L21[c] * direction.x * direction.z + sh.L2_1[c] * direction.y * direction.z) +
    2.0 * kC2 * (sh.L11[c] * direction.x + sh.L1_1[c] * direction.y + sh.L10[c] * direction.z);
}

vec3 EvaluateSphericalHarmonics(vec3 direction, vec3 worldPosition) {
    SH sh = TriLerpSurroundingProbes(direction, worldPosition);
    return vec3(SHRadiance(sh, direction, 0), SHRadiance(sh, direction, 1), SHRadiance(sh, direction, 2));
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

int shadowCascadeIndex()
{
//    vec3 projCoords = vPosInCameraSpace.xyz / vPosInCameraSpace.w;
//    // No need to transform to [0,1] range,
//    // because splits passed from client are in [-1; 1]
//
//    float fragDepth = projCoords.z;
//
//    for (int i = 0; i < uNumberOfCascades; ++i) {
//        if (fragDepth < uDepthSplits[i]) {
//            return i;
//        }
//    }

    return 0;
}

float Shadow(in vec3 N, in vec3 L, in vec3 worldPosition)
{
    int shadowCascadeIndex = shadowCascadeIndex();

    vec4 lightSpacePosition = uLightSpaceMatrices[shadowCascadeIndex] * vec4(worldPosition, 1.0);
    // perform perspective division
    vec3 projCoords = lightSpacePosition.xyz / lightSpacePosition.w;

    // Transformation to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;

    // Get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(uShadowMapArray, vec3(projCoords.xy, shadowCascadeIndex)).r;

    // Get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;

    // Check whether current frag pos is in shadow
    float bias = max(0.01 * (1.0 - dot(N, L)), 0.005);

    //    float shadow = 0.0;
    //    vec3 texelSize = 1.0 / textureSize(uShadowMapArray, 0);
    //    for(int x = -2; x <= 2; ++x) {
    //        for(int y = -2; y <= 2; ++y) {
    //            vec2 xy = projCoords.xy + vec2(x, y) * texelSize.xy;
    //            float pcfDepth = texture(uShadowMapArray, vec3(xy, shadowCascadeIndex)).r;
    //            float unbiasedDepth = currentDepth - bias;
    //            shadow += unbiasedDepth > pcfDepth && unbiasedDepth <= 1.0 ? 1.0 : 0.0;
    //        }
    //    }
    //    shadow /= 36.0;
    //    return shadow;

    float pcfDepth = texture(uShadowMapArray, vec3(projCoords.xy, shadowCascadeIndex)).r;
    float unbiasedDepth = currentDepth - bias;
    return unbiasedDepth > pcfDepth && unbiasedDepth <= 1.0 ? 1.0 : 0.0;
}

////////////////////////////////////////////////////////////
///////////////////////// Helpers //////////////////////////
////////////////////////////////////////////////////////////

vec3 ReinhardToneMapAndGammaCorrect(vec3 color) {
    vec3 mappedColor = color / (color + vec3(1.0));
    vec3 gammaCorrectedColor = pow(mappedColor, vec3(1.0 / 2.2));
    return gammaCorrectedColor;
}

vec3 ReinhardToneMap(vec3 color) {
    return color / (color + vec3(1.0));
}

vec3 GammaCorrect(vec3 color) {
    return pow(color, vec3(1.0 / 2.2));
}

vec3 LinearFromSRGB(vec3 sRGB) {
    return pow(sRGB, vec3(2.2));
}

////////////////////////////////////////////////////////////
////////////////////////// Main ////////////////////////////
////////////////////////////////////////////////////////////

void main() {
    int shadowMapsCount = uNumberOfCascades; // Stub to prevent from failing to compile on cpu side
    float split = uDepthSplits[0];  // Stub to prevent from failing to compile on cpu side

    vec3 position  = texture(uSurfelsGBuffer, vec3(vTexCoords, kGBufferIndexPosition)).rgb;
    vec3 albedo    = texture(uSurfelsGBuffer, vec3(vTexCoords, kGBufferIndexAlbedo)).rgb;
    vec3 N         = texture(uSurfelsGBuffer, vec3(vTexCoords, kGBufferIndexNormal)).rgb;
    vec3 L         = vec3(0.0);
    vec3 radiance  = vec3(0.0);
    float shadow   = 0.0;

    // Analytical lighting

    if (uLightType == kLightTypeDirectional) {
        radiance    = DirectionalLightRadiance();
        L           = -normalize(uDirectionalLight.direction);
        shadow      = Shadow(N, L, position);
    } else if (uLightType == kLightTypePoint) {
        radiance    = PointLightRadiance(N, position);
        L           = normalize(uPointLight.position - position);
    } else if (uLightType == kLightTypeSpot) {
        // Nothing to do here... yet
    }

    float NdotL = max(dot(N, L), 0.0);

    // Surfel's color (albedo) is not needed here because it's already incoded
    // in the precomputed spherical harmonics.
    // Same goes for the division by Pi, which usually takes place
    // in Lambert's diffuse component calculation - normalization has already been done
    // in the spherical harmonics precomputation step.

    vec3 diffuseRadiance = radiance * NdotL;
    
    vec3 indirectRadiance = RGB_From_YCoCg(EvaluateSphericalHarmonics(N, position));
    indirectRadiance = max(vec3(0.0), indirectRadiance);

    // Apply shadow factor
    diffuseRadiance *= 1.0 - shadow;

    vec3 finalColor = diffuseRadiance;

    if (uEnableMultibounce) { finalColor += indirectRadiance; }

    oFragColor = vec4(finalColor, 1.0);
}
