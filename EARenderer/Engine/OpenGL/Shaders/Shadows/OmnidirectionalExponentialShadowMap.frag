#version 400 core

struct PointLight {
    vec3 position;
    float farClipPlane;
};

// Uniforms
uniform float uESMFactor;
uniform PointLight uLight;

// Inputs
in vec4 vFragPosition;

// Outputs
out float oExponent;

// Functions
void main() {
    float fragToLightDistance = length(vFragPosition.xyz - uLight.position);

    // Map to [0, 1] range
    float linearDepth = fragToLightDistance / uLight.farClipPlane;
    float exponent = exp(uESMFactor * linearDepth);

    oExponent = exponent;
}
