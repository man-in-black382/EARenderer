#version 400 core

#include "GBuffer.glsl"

// Constants

const float kNormalizationFactor = 1000.0;
const float PI = 3.1415926535897932384626433832795;
const int kMaxCascades = 4;

const int kLightTypeDirectional = 0;
const int kLightTypePoint       = 1;
const int kLightTypeSpot        = 2;

const int kSHCompression333 = 0;
const int kSHCompression322 = 1;
const int kSHCompression311 = 2;

// Output

layout(location = 0) out vec4 oBaseOutput;

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
    float clipDistance;
};

struct Spotlight {
    vec3 radiantFlux; // a.k.a color
    vec3 position;
    vec3 direction;
    float cutOffAngleCos;
};

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

// Functions

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

vec3 CookTorranceBRDF(vec3 N, vec3 V, vec3 H, vec3 L, float roughness, vec3 albedo, float metallic, vec3 radiance, float shadow) {
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
    vec3 shadowedRadiance = radiance * shadow * NdotL;

    // Lambertian diffuse component with indirect light applied
    vec3 diffuse    = Kd * (albedo / PI) * shadowedRadiance;

    specular        *= shadowedRadiance;

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
        radiance    = DirectionalLightRadiance();
        L           = -normalize(uDirectionalLight.direction);
        shadow      = DirectionalExponentialShadow(worldPosition);
    }
    else if (uLightType == kLightTypePoint) {
        radiance    = PointLightRadiance(N, worldPosition);
        L           = normalize(uPointLight.position - worldPosition);
        shadow      = OmnidirectionalExponentialShadow(worldPosition);
    }
    else if (uLightType == kLightTypeSpot) {
        // Nothing to do here... yet
    }

    vec3 H = normalize(L + V);

    vec3 specularAndDiffuse = CookTorranceBRDF(N, V, H, L, roughness2, albedo, metallic, radiance, shadow);

    oBaseOutput = vec4(specularAndDiffuse, 1.0);

//    int cascade = ShadowCascadeIndex(worldPosition);
//    switch (cascade) {
//        case 0: oBaseOutput += vec4(0.5, 0.0, 0.0, 0.0); break;
//        case 1: oBaseOutput += vec4(0.0, 0.5, 0.0, 0.0); break;
//        case 2: oBaseOutput += vec4(0.0, 0.0, 0.5, 0.0); break;
//        case 3: oBaseOutput += vec4(0.5, 0.5, 0.0, 0.0); break;
//    }

    // Shrinking the output value so that it won't be clamped by additive blending
//    oBaseOutput.rgb /= kNormalizationFactor;
}
