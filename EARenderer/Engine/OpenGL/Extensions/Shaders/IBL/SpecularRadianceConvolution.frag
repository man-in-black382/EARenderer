#version 400 core

#include "Constants.glsl"
#include "Geometry.glsl"

// Uniforms

uniform samplerCube uEnvironmentMap;
uniform float       uRoughness;

// Input

in vec4 vFragPosition;

// Output

out vec3 oFragColor;

// Functions

////////////////////////////////////////////////////////////
/////////////////// Unreal Engine 4 IBL ////////////////////
////////////////////////////////////////////////////////////

// Following Epic's paper
// http://blog.selfshadow.com/publications/s2013-shading-course/karis/s2013_pbs_epic_notes_v2.pdf
// Specular lighting integral is split in two parts: irradiance and BRDF

// Irradiance integral is solved using importance sampling based on Hammersley Points
vec3 ImportanceSampleGGX(vec2 Xi, vec3 N, float roughness) {
    float a = roughness * roughness;
    
    float phi = 2.0 * PI * Xi.x;
    float cosTheta = sqrt((1.0 - Xi.y) / (1.0 + (a * a - 1.0) * Xi.y));
    float sinTheta = sqrt(1.0 - cosTheta * cosTheta);
    
    // From spherical coordinates to cartesian coordinates
    vec3 H;
    H.x = cos(phi) * sinTheta;
    H.y = sin(phi) * sinTheta;
    H.z = cosTheta;
    
    // From tangent-space vector to world-space sample vector
    vec3 up        = abs(N.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(1.0, 0.0, 0.0);
    vec3 tangent   = normalize(cross(up, N));
    vec3 bitangent = cross(N, tangent);
    
    vec3 sampleVec = tangent * H.x + bitangent * H.y + N * H.z;
    return normalize(sampleVec);
}

// Trowbridge-Reitz GGX normal distribution function
// Statistically approximates the ratio of microfacets aligned to some (halfway) vector h
//
float NormalDistributionGGX(float NdotH, float roughness) {
    float a2        = roughness * roughness;
    float NdotH2    = NdotH * NdotH;
    
    float nom       = a2;
    float denom     = (NdotH2 * (a2 - 1.0) + 1.0);
    denom           = PI * denom * denom;
    
    return nom / denom;
}

// In Epic's paper known as environment map prefiltering
vec3 IntegrateRadianceSpecular() {
    // Since it’s a microfacet model, the shape of the distribution changes based on viewing angle to the surface,
    // so we assume that this angle is zero, i.e. n = v = r. This isotropic assumption is a source of approximation
    // and it unfortunately means we don’t get lengthy reflections at grazing angles.
    // Compared with the split sum approximation, this is actually the larger source of error for IBL solution.
    // Epic Games have found weighting by cos θlk (NdotL) achieves better results
    vec3 N = normalize(vFragPosition.xyz);
    vec3 R = N;
    vec3 V = R;
    
    const uint kSampleCount = 1024;
    
    float roughness2        = uRoughness * uRoughness;
    float totalWeight       = 0.0;
    vec3 totalIrradiance    = vec3(0.0);
    
    for (uint i = 0; i < kSampleCount; ++i) {
        vec2 Xi = Hammersley2D(i, kSampleCount);
        vec3 H  = ImportanceSampleGGX(Xi, N, uRoughness);
        // Reflect H to actually sample in light direction
        vec3 L  = normalize(2.0 * dot(V, H) * H - V);
        
        float NdotL = max(dot(N, L), 0.0);
        float NdotH = max(dot(N, H), 0.0);
        float HdotV = max(dot(H, V), 0.0);
        
        if (NdotL > 0.0) {
            // Fixing bright dots on convoluted map by sampling a mip level of the environment map based on the integral's PDF and roughness:
            // https://chetanjags.wordpress.com/2015/08/26/image-based-lighting/
            
            float D             = NormalDistributionGGX(NdotH, roughness2);
            float pdf           = (D * NdotH / (4.0 * HdotV)) + 0.0001; // Propability density function
            
            float resolution    = textureSize(uEnvironmentMap, 0).x; // Resolution of source cubemap (per face)
            float saTexel       = 4.0 * PI / (6.0 * resolution * resolution);
            float saSample      = 1.0 / (float(kSampleCount) * pdf + 0.0001);
            
            float mipLevel      = uRoughness == 0.0 ? 0.0 : 0.5 * log2(saSample / saTexel);
            
            totalIrradiance     += textureLod(uEnvironmentMap, L, mipLevel).rgb * NdotL;
            totalWeight         += NdotL;
        }
    }
    
    return totalIrradiance / totalWeight;
}

void main() {
    oFragColor = IntegrateRadianceSpecular();
}
