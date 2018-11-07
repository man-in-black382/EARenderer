#version 400 core

#include "GBuffer.glsl"
#include "Lights.glsl"
#include "CookTorrance.glsl"
#include "Constants.glsl"

// Constants

const int kMaxCascades = 4;

const int kLightTypeDirectional = 0;
const int kLightTypePoint       = 1;
const int kLightTypeSpot        = 2;

// Output

layout(location = 0) out vec4 oBaseOutput;

// Input

in vec2 vTexCoords;

// Uniforms

uniform usampler2D uGBufferAlbedoRoughnessMetalnessAONormal;
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
uniform mat4 uLightSpaceMatrices[kMaxCascades];
uniform int uDepthSplitsAxis;
uniform float uDepthSplits[kMaxCascades];
uniform int uNumberOfCascades;
uniform float uESMFactor;
uniform sampler2D uDirectionalShadowMap;
uniform samplerCubeArray uOmnidirectionalShadowMaps;
uniform int uOmnidirectionalShadowMapIndex;

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

float OmnidirectionalExponentialShadow(vec3 worldPosition) {
    vec3 lightToFrag = worldPosition - uPointLight.position;
    float occluder = texture(uOmnidirectionalShadowMaps, vec4(lightToFrag, float(uOmnidirectionalShadowMapIndex))).r;
    float linearZ = length(lightToFrag) / uPointLight.clipDistance;
    float receiver = exp(-uESMFactor * linearZ);
    float visibility = clamp(occluder * receiver, 0.0, 1.0);

    return visibility;
}

////////////////////////////////////////////////////////////
////////////////////////// Main ////////////////////////////
////////////////////////////////////////////////////////////

void main() {
    GBuffer gBuffer     = DecodeGBuffer(uGBufferAlbedoRoughnessMetalnessAONormal, vTexCoords);

    vec3 worldPosition  = ReconstructWorldPosition(uGBufferHiZBuffer, vTexCoords, uCameraViewInverse, uCameraProjectionInverse);
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
        radiance    = DirectionalLightRadiance(uDirectionalLight);
        L           = -normalize(uDirectionalLight.direction);
        shadow      = DirectionalExponentialShadow(worldPosition);
    }
    else if (uLightType == kLightTypePoint) {
        radiance    = PointLightRadiance(uPointLight, worldPosition);
        L           = normalize(uPointLight.position - worldPosition);
        shadow      = OmnidirectionalExponentialShadow(worldPosition);
    }
    else if (uLightType == kLightTypeSpot) {
        // Nothing to do here... yet
    }

    vec3 H = normalize(L + V);

    vec3 specularAndDiffuse = CookTorranceBRDF(N, V, H, L, roughness2, albedo, metallic, radiance, shadow);

    // Shrinking the output value so that it won't be clamped by additive blending
    oBaseOutput = vec4(specularAndDiffuse / HDRNormalizationFactor, 1.0);

//    int cascade = ShadowCascadeIndex(worldPosition);
//    switch (cascade) {
//        case 0: oBaseOutput += vec4(0.5, 0.0, 0.0, 0.0); break;
//        case 1: oBaseOutput += vec4(0.0, 0.5, 0.0, 0.0); break;
//        case 2: oBaseOutput += vec4(0.0, 0.0, 0.5, 0.0); break;
//        case 3: oBaseOutput += vec4(0.5, 0.5, 0.0, 0.0); break;
//    }


//    oBaseOutput.rgb /= kNormalizationFactor;
}
