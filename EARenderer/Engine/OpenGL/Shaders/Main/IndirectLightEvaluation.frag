#version 400 core

//#define PROBE_SH_COMPRESSION_322

#include "GBuffer.glsl"
#include "ColorSpace.glsl"
#include "DiffuseLightProbes.glsl"
#include "Constants.glsl"
#include "CookTorrance.glsl"

// Output

layout(location = 0) out vec4 oOutputColor;

// Input

in vec2 vTexCoords;

// Uniforms
uniform usampler2D uGBufferAlbedoRoughnessMetalnessAONormal;
uniform sampler2D uGBufferHiZBuffer;

uniform vec3 uCameraPosition;
uniform mat4 uCameraViewInverse;
uniform mat4 uCameraProjectionInverse;
uniform mat4 uWorldBoudningBoxTransform;

uniform uint uSettingsBitmask;

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
    // to-light vector is replaced by the fragment's normal.
    //
    // !!! (This could probably be improved by storing and using reflected point's normal alongside reflection buffer) !!!
    //
    vec3 H = normalize(N + V);

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
    
    // Filter out negative values which can occur from time to time when dealing with spherical harmonics
    indirectRadiance = max(indirectRadiance, vec3(0.0));

    //    indirectRadiance *= isGlobalIlluminationEnabled() ? 1.0 : 0.0;
    indirectRadiance *= Kd;

    vec3 fragmentColor = indirectRadiance * albedo * ao;

    oOutputColor = vec4(fragmentColor / HDRNormalizationFactor, 1.0);
}
