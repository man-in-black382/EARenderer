#version 400 core

layout (location = 0) in vec3 iPosition;
layout (location = 1) in vec3 iNormal;
layout (location = 2) in vec3 iAlbedo;
layout (location = 3) in vec2 iUV;
layout (location = 4) in float iArea;

out vec3 gNormal;
out float gArea;

void main() {
    gNormal = iNormal;
    gArea = iArea;
    gl_Position = vec4(iPosition, 1.0);
}
