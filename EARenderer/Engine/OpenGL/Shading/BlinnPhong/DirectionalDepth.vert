#version 400 core

uniform mat4 uModelMatrix;
uniform mat4 uViewProjectionMatrix;

layout (location = 0) in vec4 iPosition;

void main() {
    gl_Position = uViewProjectionMatrix * uModelMatrix * iPosition;
}

