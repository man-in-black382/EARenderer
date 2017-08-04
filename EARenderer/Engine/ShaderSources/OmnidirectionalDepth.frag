#version 400 core

struct PointLight {
    vec3 position;
    float farClipPlane;
};

uniform PointLight uLight;

in vec4 vFragPosition;

out vec4 fragColor;

void main()
{
    float fragToLightDistance = length(vFragPosition.xyz - uLight.position);
    // Map to [0, 1] depth range
    gl_FragDepth = fragToLightDistance / uLight.farClipPlane;
}
