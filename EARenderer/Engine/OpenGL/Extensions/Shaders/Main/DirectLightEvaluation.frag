#version 400 core

#include "GBuffer.glsl"
#include "Lights.glsl"
#include "CookTorrance.glsl"
#include "Constants.glsl"
#include "DirectionalShadows.glsl"
#include "OmnidirectionalShadows.glsl"

// Constants

const int kLightTypeDirectional = 0;
const int kLightTypePoint       = 1;
const int kLightTypeSpot        = 2;

// Output

layout(location = 0) out vec4 oColor;

// Input

in vec2 vTexCoords;

// Uniforms

uniform usampler2D uMaterialData;
uniform sampler2D uGBufferHiZBuffer;

uniform vec3 uCameraPosition;
uniform mat4 uCameraViewInverse;
uniform mat4 uCameraProjectionInverse;

uniform DirectionalLight uDirectionalLight;
uniform PointLight uPointLight;
uniform Spotlight uSpotlight;

uniform int uLightType;
uniform uint uSettingsBitmask;
uniform float uParallaxMappingStrength;

// Shadow mapping
uniform mat4 uCSMSplitSpaceMat;
uniform mat4 uLightSpaceMatrices[MaximumShadowCascadesCount];
uniform int uDepthSplitsAxis;
uniform float uDepthSplits[MaximumShadowCascadesCount];
uniform int uNumberOfCascades;
uniform sampler2DArrayShadow uDirectionalShadowMapsComparisonSampler;
uniform samplerCubeShadow uOmnidirectionalShadowMapComparisonSampler;
uniform sampler2D uPenumbra;

// Functions

// Settings
bool areMaterialsEnabled()          { return bool((uSettingsBitmask >> 4u) & 1u); }
bool isGlobalIlluminationEnabled()  { return bool((uSettingsBitmask >> 3u) & 1u); }
bool isLightMultibounceEnabled()    { return bool((uSettingsBitmask >> 2u) & 1u); }
bool isMeshRenderingEnabled()       { return bool((uSettingsBitmask >> 1u) & 1u); }
bool isParallaxMappingEnabled()     { return bool((uSettingsBitmask >> 0u) & 1u); }

////////////////////////////////////////////////////////////
//////////////////// Lighting equation /////////////////////
////////////////////////////////////////////////////////////

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
////////////////////////// Main ////////////////////////////
////////////////////////////////////////////////////////////

vec4 ProcessCookTorranceMaterial(uvec4 materialData, vec3 worldPosition) {
    GBufferCookTorrance gBuffer = DecodeGBufferCookTorrance(materialData);

    float roughness     = gBuffer.roughness;

    // Based on observations by Disney and adopted by Epic Games
    // the lighting looks more correct squaring the roughness
    // in both the geometry and normal distribution function.
    float roughness2    = roughness * roughness;

    float metallic      = gBuffer.metalness;
    vec3 albedo         = gBuffer.albedo;
    vec3 N              = gBuffer.normal;
    vec3 V              = normalize(uCameraPosition - worldPosition);
    vec3 L              = vec3(0.0);
    vec3 radiance       = vec3(0.0);
    float shadow        = 0.0;

    if (!areMaterialsEnabled()) {
        albedo = vec3(1.0);
        roughness = 1.0;
        roughness2 = 1.0;
        metallic = 0.0;
    }

    // Analytical lighting
    if (uLightType == kLightTypeDirectional) {
        radiance = DirectionalLightRadiance(uDirectionalLight);
        L  = -normalize(uDirectionalLight.direction);
        int cascade = ShadowCascadeIndex(worldPosition, uCSMSplitSpaceMat, uDepthSplitsAxis, uDepthSplits);
        float penumbra = texture(uPenumbra, vTexCoords).r;
        shadow = DirectionalShadow(worldPosition, N, L, cascade, uLightSpaceMatrices, uDirectionalShadowMapsComparisonSampler, penumbra);
    }
    else if (uLightType == kLightTypePoint) {
        radiance = PointLightRadiance(uPointLight, worldPosition);
        L = normalize(uPointLight.position - worldPosition);
        float penumbra = texture(uPenumbra, vTexCoords).r;
        shadow = OmnidirectionalShadow(worldPosition, N, uPointLight, uOmnidirectionalShadowMapComparisonSampler, penumbra);
    }
    else if (uLightType == kLightTypeSpot) {
        // Nothing to do here... yet
    }

    vec3 H = normalize(L + V);
    vec3 specularAndDiffuse = CookTorranceBRDF(N, V, H, L, roughness2, albedo, metallic, radiance, shadow);

    return vec4(specularAndDiffuse, 1.0);
//    return vec4(vec3(shadow), 1.0);
}

vec4 ProcessEmissiveMaterial(uvec4 materialData) {
    GBufferEmissive gBuffer = DecodeGBufferEmissive(materialData);
    return vec4(gBuffer.emission, 1.0);
}

void main() {
    uvec4 materialData = texture(uMaterialData, vTexCoords);
    vec3 worldPosition = ReconstructWorldPosition(uGBufferHiZBuffer, vTexCoords, uCameraViewInverse, uCameraProjectionInverse);

    switch (DecodeGBufferMaterialType(materialData)) {
        case MaterialTypeCookTorrance: oColor = ProcessCookTorranceMaterial(materialData, worldPosition); break;
        case MaterialTypeEmissive: oColor = ProcessEmissiveMaterial(materialData); break;
    }

    // Shrinking the output value so that it won't be clamped by additive blending
    oColor.rgb /= HDRNormalizationFactor;

//    int cascade = ShadowCascadeIndex(worldPosition);
//    switch (cascade) {
//        case 0: oColor += vec4(0.5, 0.0, 0.0, 0.0); break;
//        case 1: oColor += vec4(0.0, 0.5, 0.0, 0.0); break;
//        case 2: oColor += vec4(0.0, 0.0, 0.5, 0.0); break;
//        case 3: oColor += vec4(0.5, 0.5, 0.0, 0.0); break;
//    }
}
