#version 400 core

// Constants

const float PI = 3.1415926535897932384626433832795;

// Uniforms

uniform samplerCube uEnvironmentMap;

// Input

in vec2 vTexCoords;

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

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float a = roughness;
    // K is different from K in Cook-Torrance shader for analytical lights
    float k = (a * a) / 2.0;
    
    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;
    
    return nom / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);
    
    return ggx1 * ggx2;
}

vec2 IntegrateBRDF(float NdotV, float roughness)
{
    vec3 V;
    V.x = sqrt(1.0 - NdotV * NdotV);
    V.y = 0.0;
    V.z = NdotV;
    
    float scale = 0.0;
    float bias = 0.0;
    
    vec3 N = vec3(0.0, 0.0, 1.0);
    
    const uint kSampleCount = 1024;
    
    for(uint i = 0; i < kSampleCount; ++i)
    {
        vec2 Xi = Hammersley2D(i, kSampleCount);
        vec3 H  = ImportanceSampleGGX(Xi, N, roughness);
        vec3 L  = normalize(2.0 * dot(V, H) * H - V);
        
        float NdotL = max(L.z, 0.0);
        float NdotH = max(H.z, 0.0);
        float VdotH = max(dot(V, H), 0.0);
        
        if(NdotL > 0.0)
        {
            float G = GeometrySmith(N, V, L, roughness);
            float G_Vis = (G * VdotH) / (NdotH * NdotV);
            float Fc = pow(1.0 - VdotH, 5.0);
            
            scale += (1.0 - Fc) * G_Vis;
            bias += Fc * G_Vis;
        }
    }

    return vec2(scale, bias) / kSampleCount;
}

void main() {
    oFragColor = vec4(IntegrateBRDF(), 0.0, 1.0);
}
