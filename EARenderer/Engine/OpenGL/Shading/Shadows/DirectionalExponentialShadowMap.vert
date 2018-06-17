#version 400 core

uniform mat4 uModelViewProjectionMatrix;

layout (location = 0) in vec4 iPosition;

out vec3 vFragClipSpacePosition;

void main() {
    vec4 lightSpacePosition = uModelViewProjectionMatrix * iPosition;
    vFragClipSpacePosition = lightSpacePosition.xyz;
    gl_Position = lightSpacePosition;
}

