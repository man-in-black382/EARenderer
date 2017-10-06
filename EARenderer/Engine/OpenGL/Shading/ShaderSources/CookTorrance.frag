#version 400 core

// Constants

const float PI = 3.1415926535897932384626433832795;

// Output

out vec4 outputFragColor;

// Input

in vec3 vTexCoords;
in vec3 vWorldPosition;
in vec3 vNormal;

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
}

struct Material {
    uniform vec3 albedo;
    uniform float metallic;
    uniform float roughness;
    uniform float AO; // Ambient occlusion
}

uniform vec3 uCameraPosition;

uniform DirectionalLight uDirectionalLight;
uniform PointLight uPointLight;
uniform Spotlight uSpotlight;
uniform Material uMaterial;

// Functions

// The Fresnel equation describes the ratio of surface reflection at different surface angles.
// (This is an approximation of Fresnels' equation, called Fresnel-Schlick)
// Describes the ratio of light that gets reflected over the light that gets refracted,
// which varies over the angle we're looking at a surface.
//
vec3 FresnelSchlick(vec3 N, vec3 H) {
    // F0 - base reflectivity of a surface, a.k.a. surface reflection at zero incidence
    // or how much the surface reflects if looking directly at the surface
    vec3 F0 = vec3(0.04); // 0.04 is a commonly used constant for dielectrics
    F0 = mix(F0, uMaterial.albedo, uMaterial.metallic);
    
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
    float r         = (roughness + 1.0);
    
    // Here k is a remapping of roughness based on whether we're using the geometry function for either direct lighting or IBL lighting
    float Kdirect   = (r * r) / 8.0;
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

float CookTorranceBRDF(vec3 N, vec3 V, vec3 H, vec3 L, float roughness) {
    float NdotL     = max(dot(N, L), 0.0);
    float NdotV     = max(dot(N, V), 0.0);
    
    float NDF       = NormalDistributionGGX(N, H, roughness);
    float G         = GeometrySmith(NdotL, NdotV, roughness);
    vec3 F          = FresnelSchlick(N, H);

    vec3 nom        = NDF * G * F;
    float denom     = 4.0 * NdotL * NdotV + 0.001; // Add 0.001 to prevent division by 0
    vec3 specular   = nom / denom;
    
    
}

vec3 PointLightRadiance(vec3 N) {
    vec3 Wi                 = normalize(uPointLight.position - vWorldPosition);     // To light vector
    float cosTheta          = max(dot(N, Wi), 0.0);                                 // Angular attenuation
    float distance          = length(Wi);                                           // Distance from fragment to light
    float attenuation       = 1.0 / (distance * distance);                          // How much enegry has light lost at current distance
    
    return uPointLight.radiantFlux * attenuation * cosTheta;
}

void main() {
    vec3 N = normalize(vNormal);
    vec3 V = normalize(uCameraPosition - vWorldPosition);
//    vec3 L =
    
    // Based on observations by Disney and adopted by Epic Games
    // the lighting looks more correct squaring the roughness
    // in both the geometry and normal distribution function.
    float roughness = uMaterial.roughness * uMaterial.roughness;
}
