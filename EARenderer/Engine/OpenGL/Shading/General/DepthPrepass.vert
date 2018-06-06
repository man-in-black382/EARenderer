#version 400 core

// Constants
const int kMaxCascades = 4;

// Attributes

layout (location = 0) in vec4 iPosition;
layout (location = 1) in vec3 iTexCoords;
layout (location = 2) in vec2 iLightmapCoords;
layout (location = 3) in vec3 iNormal;
layout (location = 4) in vec3 iTangent;
layout (location = 5) in vec3 iBitangent;

// Uniforms

uniform mat4 uModelMat;
uniform mat4 uCameraSpaceMat;

void main() {
    vec4 worldPosition = uModelMat * iPosition;
    gl_Position = uCameraSpaceMat * worldPosition;
}
