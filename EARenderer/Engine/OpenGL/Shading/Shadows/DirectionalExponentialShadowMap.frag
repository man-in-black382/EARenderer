#version 400 core

// Uniforms

uniform float uESMFactor;

// Input

in vec4 vFragClipSpacePosition;

// Output

out vec4 oFragColor;

void main() {
    oFragColor = vec4(exp(uESMFactor * vFragClipSpacePosition.z), 0.0, 0.0, 1.0);
}
