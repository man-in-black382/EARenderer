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
uniform mat4 uNormalMat;
uniform mat4 uCameraSpaceMat;
uniform vec3 uCameraPosition;

/* Shadow mapping */
uniform mat4 uLightSpaceMatrices[kMaxCascades];
uniform int uNumberOfCascades;

// Output

out vec3 vTexCoords;
out vec2 vLightmapCoords;
out vec3 vWorldPosition;
out mat3 vTBN;
out vec4 vPosInLightSpace[kMaxCascades];
out vec4 vPosInCameraSpace;

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
    
    vTexCoords = vec3(iTexCoords.s, iTexCoords.t, iTexCoords.r);
    vLightmapCoords = iLightmapCoords;
    vWorldPosition = worldPosition.xyz;
    vTBN = orthogonalTBN();
    
    for (int i = 0; i < uNumberOfCascades; ++i) {
        vPosInLightSpace[i] = uLightSpaceMatrices[i] * worldPosition;
    }
    
    vPosInCameraSpace = uCameraSpaceMat * worldPosition;
    gl_Position = vPosInCameraSpace;
    
}
