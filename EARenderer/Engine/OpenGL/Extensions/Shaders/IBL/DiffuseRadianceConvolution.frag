#version 400 core

#include "Constants.glsl"
#include "Geometry.glsl"

// Uniforms

uniform samplerCube uEnvironmentMap;

// Input

in vec4 vFragPosition;

// Output

out vec3 oFragColor;

// Functions

vec3 IntegrateRadianceDiffuse() {
    vec3 N      = normalize(vFragPosition.xyz);
    vec3 up     = vec3(0.0, 1.0, 0.0);
    vec3 right  = normalize(cross(up, N));
    up          = normalize(cross(N, right));
    
    vec3 irradiance     = vec3(0.0);
    float samplesCount  = 0.0;
    
    const float kSampleDelta = 0.025;
    const float twoPI        = 2.0 * PI;
    const float halfPI       = 0.5 * PI;
    
    for(float phi = 0.0; phi < twoPI; phi += kSampleDelta) { // Phi - azimuth (horizontal) angle
        for(float theta = 0.0; theta < halfPI; theta += kSampleDelta) { // Theta - zenith (vertical) angle
            // Spherical to cartesian (in tangent space)
            vec3 tangentSample = vec3(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
            // Tangent space to world
            vec3 sampleVec = tangentSample.x * right + tangentSample.y * up + tangentSample.z * N;
            
            // Scale the sampled color value by cos(theta) due to the light being weaker at larger angles
            // And by sin(theta) to account for the smaller sample areas in the higher hemisphere areas
            irradiance += textureLod(uEnvironmentMap, sampleVec, 0).rgb * cos(theta) * sin(theta);
            samplesCount += 1.0;
        }
    }
    
    return PI * irradiance / samplesCount;
}

void main() {
//    oFragColor = textureLod(uEnvironmentMap, normalize(vFragPosition.xyz), 0).rgb;
    oFragColor = IntegrateRadianceDiffuse();
//    oFragColor = normalize(vFragPosition.xyz);
//    oFragColor = vec3(0.0, 1.0, 0.0);
}
