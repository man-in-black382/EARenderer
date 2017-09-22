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

uniform vec3 uCameraPosition;
uniform mat4 uCameraSpaceMat;

uniform int uNumberOfCascades;

// Output

out vec3 oNormal;
out vec3 oToCamera;
out vec3 oTexCoords;
out vec3 oTangent;
out vec3 oBitangent;
out vec4 oPosInLightSpace[kMaxCascades];
out vec4 oPosInCameraSpace;
out mat3 oTBN;

void main() {
    vec4 worldPosition = uModelMat * iPosition;
    
    // Flip texture Y coordinate, because loaded images are flipped vertically
    oTexCoords = vec3(iTexCoords.s, 1.0 - iTexCoords.t, iTexCoords.r);
    oToCamera = normalize(uCameraPosition - worldPosition.xyz);
    oPosInCameraSpace = uCameraSpaceMat * worldPosition;
    
    oTangent = normalize(uNormalMat * vec4(iTangent, 0.0)).xyz;
    oBitangent = normalize(uNormalMat * vec4(iBitangent, 0.0)).xyz;
    oNormal = normalize(uNormalMat * vec4(iNormal, 0.0)).xyz;
    oTBN = mat3(oTangent, oBitangent, oNormal);
    
    for (int i = 0; i < uNumberOfCascades; ++i) {
        oPosInLightSpace[i] = uLightSpaceMatrices[i] * worldPosition;
    }
    
    gl_Position = uCameraSpaceMat * worldPosition;
}
