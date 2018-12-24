#version 400 core

layout (location = 0) in vec3 iPosition;
layout (location = 1) in vec3 iNormal;
layout (location = 2) in vec4 iAlbedo;
layout (location = 3) in float iArea;

uniform int uSurfelGroupOffset;

out vec3 gNormal;
out vec3 gAlbedo;
out float gArea;
flat out int gSurfelIndex;

void main() {
    gNormal = iNormal;
    gAlbedo = iAlbedo.rgb;
    gArea = iArea;
    gSurfelIndex = uSurfelGroupOffset + gl_VertexID;
    gl_Position = vec4(iPosition, 1.0);
}
