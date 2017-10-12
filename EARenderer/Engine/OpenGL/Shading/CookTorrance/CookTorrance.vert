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
out mat3 vTBN;

// Functions

// Build TBN matrix as-is
mat3 TBN() {
    vec3 T = normalize(uNormalMat * vec4(iTangent, 0.0)).xyz;
    vec3 B = normalize(uNormalMat * vec4(iBitangent, 0.0)).xyz;
    vec3 N = normalize(uNormalMat * vec4(iNormal, 0.0)).xyz;
    return mat3(T, B, N);
}

// Build TBN matrix and make sure it's orthogonal by using Gram-Schmidt's approach
mat3 orthogonalTBN() {
    vec3 T = normalize(uNormalMat * vec4(iTangent, 0.0)).xyz;
    vec3 N = normalize(uNormalMat * vec4(iNormal, 0.0)).xyz;
    // re-orthogonalize T with respect to N
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T);
    return mat3(T, B, N);
}

void main() {
    vec4 worldPosition = uModelMat * iPosition;
    
    // Flip texture Y coordinate, because loaded images are flipped vertically
    vTexCoords = vec3(iTexCoords.s, 1.0 - iTexCoords.t, iTexCoords.r);
    vWorldPosition = worldPosition.xyz;
    vTBN = orthogonalTBN();
    
//    oToCamera = normalize(uCameraPosition - worldPosition.xyz);
    
    gl_Position = uCameraSpaceMat * worldPosition;
    
}
