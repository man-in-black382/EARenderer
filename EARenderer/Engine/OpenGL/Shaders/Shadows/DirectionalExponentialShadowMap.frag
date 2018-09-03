#version 400 core

// Constants

// Uniforms

uniform float uESMFactor;

// Input

in vec3 vFragClipSpacePosition;

// Output

out vec4 oFragColor;

// Functions

void main() {
    // Transform to [0; 1] range
    float linearDepth = vFragClipSpacePosition.z * 0.5 + 0.5;
    float exponent = exp(uESMFactor * linearDepth);

    // Color masking will ensure that exponent will be written into correct channel
    // r for cascade 0, g - for 1 and so on
    oFragColor = vec4(exponent);
}
