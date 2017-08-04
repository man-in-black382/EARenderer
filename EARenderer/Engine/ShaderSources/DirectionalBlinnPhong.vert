#version 400 core

layout (location = 0) in vec4 iPosition;
layout (location = 1) in vec3 iTexCoords;
layout (location = 2) in vec3 iNormal;

uniform mat4 uCameraSpaceMat;
uniform mat4 uModelMat;
uniform mat4 uNormalMat;
uniform mat4 uLightSpaceMat;

uniform vec3 uCameraPosition;

out vec3 oNormal;
out vec3 oToCamera;
out vec3 oTexCoords;
out vec4 oPosInLightSpace;

void main() {
    vec4 worldPosition = uModelMat * iPosition;
    
    // Flip texture Y coordinate, because loaded images are flipped vertically
    oTexCoords = vec3(iTexCoords.s, 1.0 - iTexCoords.t, iTexCoords.r);
    oNormal = normalize(uNormalMat * vec4(iNormal, 0.0)).xyz;
    oToCamera = normalize(uCameraPosition - worldPosition.xyz);
    oPosInLightSpace = uLightSpaceMat * worldPosition;
    
    gl_Position = uCameraSpaceMat * worldPosition;
//    gl_Position = vec4(((gl_VertexID & 1) << 2) - 1, (gl_VertexID & 2) * 2 - 1, 1.0, 1.0);
}
