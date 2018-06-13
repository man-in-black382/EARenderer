#version 400 core

uniform mat4 uModelMatrix;
uniform mat4 uViewProjectionMatrix;

layout (location = 0) in vec4 iPosition;

out vec4 vFragClipSpacePosition;

void main() {
    vFragClipSpacePosition = uViewProjectionMatrix * uModelMatrix * iPosition;
    gl_Position = vFragClipSpacePosition;
}

