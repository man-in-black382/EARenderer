#version 400 core

layout (location = 0) in vec3 iPosition;
layout (location = 1) in vec3 iNormal;

uniform mat4 uModelViewProjection;

out vec3 gNormal;

void main() {
    gNormal = iNormal;
    gl_Position = uModelViewProjection * vec4(iPosition, 1.0);
}
