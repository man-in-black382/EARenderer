#version 400 core

#include "GBuffer.glsl"
#include "ColorSpace.glsl"
#include "DiffuseLightProbes.glsl"

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
////////////////////////// Main ////////////////////////////
////////////////////////////////////////////////////////////

void main() {
    GBuffer gBuffer     = DecodeGBuffer(uGBufferAlbedoRoughnessMetalnessAONormal, vTexCoords);

    vec3 worldPosition  = ReconstructWorldPosition(uGBufferHiZBuffer, vTexCoords, uCameraViewInverse, uCameraProjectionInverse);

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

    vec3 indirectRadiance = EvaluateDiffuseLightProbes(uGridSHMap0,
                                                       uGridSHMap1,
                                                       uGridSHMap2,
                                                       uGridSHMap3,
                                                       uProbePositions,
                                                       N,
                                                       worldPosition,
                                                       uWorldBoudningBoxTransform);

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
