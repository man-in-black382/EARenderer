#version 400 core

// Attributes

layout (location = 0) in vec4 iPosition;
layout (location = 1) in vec3 iTexCoords;
layout (location = 2) in vec3 iNormal;
layout (location = 3) in vec3 iTangent;
layout (location = 4) in vec3 iBitangent;

// Uniforms

uniform mat4 uModelMat;
uniform mat4 uNormalMat;
uniform mat4 uCameraSpaceMat;
uniform vec3 uCameraPosition;

// Output

out vec3 vTexCoords;
out vec3 vWorldPosition;
out vec3 vNormal;

// Functions

void main() {
    vec4 worldPosition = uModelMat * iPosition;
    
    // Flip texture Y coordinate, because loaded images are flipped vertically
    vTexCoords = vec3(iTexCoords.s, 1.0 - iTexCoords.t, iTexCoords.r);
    vWorldPosition = worldPosition.xyz;
//    oToCamera = normalize(uCameraPosition - worldPosition.xyz);
    
    vNormal = iNormal;

    gl_Position = uCameraSpaceMat * worldPosition;
    
}
