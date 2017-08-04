#version 400 core

// Attributes

layout (location = 0) in vec4 iPosition;
layout (location = 1) in vec3 iTexCoords;
layout (location = 2) in vec3 iNormal;

// Uniforms

struct Light {
    vec3 position;
    vec3 color;
    float farClipPlane;
};

struct Camera {
    vec3 position;
    mat4 viewProjMat;
};

uniform Light uLight;
uniform Camera uCamera;
uniform mat4 uModelMat;
uniform mat4 uNormalMat;

// Output

out vec3 oNormal;
out vec3 oToLight;
out vec3 oToCamera;
out vec3 oTexCoords;
out vec4 oFragWorldPos;

void main() {
    oFragWorldPos = uModelMat * iPosition;
    
    // Flip texture Y coordinate, because loaded images are flipped vertically
    oTexCoords = vec3(iTexCoords.s, 1.0 - iTexCoords.t, iTexCoords.r);
    oNormal = normalize(uNormalMat * vec4(iNormal, 0.0)).xyz;
    oToLight = normalize(uLight.position - oFragWorldPos.xyz);
    oToCamera = normalize(uCamera.position - oFragWorldPos.xyz);
    
    gl_Position = uCamera.viewProjMat * oFragWorldPos;
}
