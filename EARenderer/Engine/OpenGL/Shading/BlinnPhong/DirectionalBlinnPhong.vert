#version 400 core

// Constants

const int kMaxCascades = 4;

// Input

layout (location = 0) in vec4 iPosition;
layout (location = 1) in vec3 iTexCoords;
layout (location = 2) in vec3 iNormal;
layout (location = 3) in vec3 iTangent;
layout (location = 4) in vec3 iBitangent;

// Uniforms

uniform mat4 uModelMat;
uniform mat4 uNormalMat;
uniform mat4 uLightSpaceMatrices[kMaxCascades];
uniform mat4 uCameraSpaceMat;
uniform vec3 uCameraPosition;
uniform int uNumberOfCascades;

// Output

out vec3 oNormal;
out vec3 oToCamera;
out vec3 oTexCoords;
out vec4 oPosInLightSpace[kMaxCascades];
out vec4 oPosInCameraSpace;
out mat3 oTBN;

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
    oTexCoords = vec3(iTexCoords.s, 1.0 - iTexCoords.t, iTexCoords.r);
    oToCamera = normalize(uCameraPosition - worldPosition.xyz);
    oPosInCameraSpace = uCameraSpaceMat * worldPosition;

    oTBN = orthogonalTBN();
    oNormal = iNormal;
    
    for (int i = 0; i < uNumberOfCascades; ++i) {
        oPosInLightSpace[i] = uLightSpaceMatrices[i] * worldPosition;
    }
    
    gl_Position = uCameraSpaceMat * worldPosition;
}
