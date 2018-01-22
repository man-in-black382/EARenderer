#version 400 core

layout (location = 0) in vec3 iPosition;
layout (location = 1) in vec3 iNormal;
layout (location = 2) in vec3 iAlbedo;
layout (location = 3) in vec2 iUV;
layout (location = 4) in float iArea;

uniform mat4 uModelViewProjection;

out vec3 gNormal;
out float gArea;

// TBN matrix used to rotate triangle fan (surfel disk) in geometry shader
//mat3 TBN() {
//    vec3 T = normalize(uNormalMat * vec4(iTangent, 0.0)).xyz;
//    vec3 N = normalize(uNormalMat * vec4(iNormal, 0.0)).xyz;
//   // re-orthogonalize T with respect to N
//    T = normalize(T - dot(T, N) * N);
//    vec3 B = cross(N, T);
//    return mat3(T, B, N);
//}

void main() {
    gNormal = iNormal;
    gArea = iArea;
    gl_Position = uModelViewProjection * vec4(iPosition, 1.0);
}
