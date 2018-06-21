#version 400 core

const int kMaxCascades = 4;

// Inputs

layout (location = 0) in vec4 iPosition;

// Uniforms

uniform mat4 uModelMatrix;
uniform mat4 uCameraMatrix;
uniform mat4 uLightSpaceMatrices[kMaxCascades];

// Outputs

out vec3 vFragClipSpacePosition;
out vec4 vPosInCameraSpace;

// Functions

void main() {
    vec4 worldPosition = uModelMatrix * iPosition;
    vec4 lightSpacePosition = uLightSpaceMatrices[gl_InstanceID] * worldPosition;

    vFragClipSpacePosition = lightSpacePosition.xyz;
    vPosInCameraSpace = uCameraMatrix * worldPosition;

    gl_Position = lightSpacePosition;
}

