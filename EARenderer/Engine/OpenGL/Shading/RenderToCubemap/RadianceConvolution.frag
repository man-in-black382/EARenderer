#version 400 core

// Constants

const float PI = 3.1415926535897932384626433832795;

// Uniforms

uniform samplerCube uEnvironmentMap;
uniform float       uRoughness;
uniform int         uRoughnessSampleCount;

// Input

in vec4 vFragPosition;

// Output

out vec4 oFragColor;

// Functions

////////////////////////////////////////////////////////////
/////////////////// Unreal Engine 4 IBL ////////////////////
////////////////////////////////////////////////////////////

// Hammersley Points on the Hemisphere
// http://holger.dammertz.org/stuff/notes_HammersleyOnHemisphere.html

float RadicalInverse_VdC(uint bits) {
    bits = (bits << 16u) | (bits >> 16u);
    bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
    bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
    bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
    bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
    return float(bits) * 2.3283064365386963e-10; // / 0x100000000
}

// Generates a uniformly distributed and stochasticlooking sampling pattern at low computational cost
// Used for quasi-Monte Carlo integration
vec2 Hammersley2D(uint i, uint N) {
    return vec2(float(i) / float(N), RadicalInverse_VdC(i));
}

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

void main() {
    // Since it’s a microfacet model, the shape of the distribution changes based on viewing angle to the surface,
    // so we assume that this angle is zero, i.e. n = v = r. This isotropic assumption is a source of approximation
    // and it unfortunately means we don’t get lengthy reflections at grazing angles.
    // Compared with the split sum approximation, this is actually the larger source of error for IBL solution.
    // Epic Games have found weighting by cos θlk (NdotL) achieves better results
    vec3 N = normalize(vFragPosition.xyz);
    vec3 R = N;
    vec3 V = R;
    
    const uint kSampleCount = 1024;
    
    float totalWeight       = 0.0;
    vec3 totalIrradiance    = vec3(0.0);
    
    for (uint i = 0; i < kSampleCount; ++i) {
        vec2 Xi = Hammersley2D(i, kSampleCount);
        vec3 H  = ImportanceSampleGGX(Xi, N, uRoughness);
        // Reflect H to actually sample in light direction
        vec3 L  = normalize(2.0 * dot(V, H) * H - V);
        
        float NdotL = max(dot(N, L), 0.0);
        if (NdotL > 0.0) {
            totalIrradiance += texture(uEnvironmentMap, L).rgb * NdotL;
            totalWeight     += NdotL;
        }
    }
    
    totalIrradiance = totalIrradiance / totalWeight;
    
    oFragColor = vec4(totalIrradiance, 1.0);
    
    // DEBUG
//    oFragColor = vec4(vFragPosition.xyz, 1.0);
}
