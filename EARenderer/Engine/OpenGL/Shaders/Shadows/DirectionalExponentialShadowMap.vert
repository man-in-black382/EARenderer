#version 400 core

const int kMaxCascades = 4;

// Inputs

layout (location = 0) in vec4 iPosition;

// Uniforms

uniform mat4 uModelMatrix;
uniform mat4 uLightSpaceMatrix;

// Outputs

out vec3 vFragClipSpacePosition;

// Functions

void main() {
    vec4 worldPosition = uModelMatrix * iPosition;
    vec4 lightSpacePosition = uLightSpaceMatrix * worldPosition;

    vFragClipSpacePosition = lightSpacePosition.xyz;

    gl_Position = lightSpacePosition;
}

