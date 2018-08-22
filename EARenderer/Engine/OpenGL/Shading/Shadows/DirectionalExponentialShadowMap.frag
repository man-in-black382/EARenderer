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
    float linearDepth = vFragClipSpacePosition.z;
    float exponent = exp(uESMFactor * linearDepth);

    // Color masking will ensure that exponent will be written into correct channel
    // r fo cascade 0, g - for 1 and so on
    oFragColor = vec4(exponent);
//    oFragColor = vec4(1.0, 0.5, 0.0, 0.0);
}
