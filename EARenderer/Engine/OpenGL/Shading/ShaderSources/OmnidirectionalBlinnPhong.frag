#version 400 core

// Output

out vec4 outputFragColor;

// Input

in vec3 oNormal;
in vec3 oToLight;
in vec3 oToCamera;
in vec3 oTexCoords;
in vec4 oFragWorldPos;

// Uniforms

struct Light {
    vec3 position;
    vec3 color;
    float farClipPlane;
};

struct Material {
    sampler2D diffuseTexture;
    vec3 ambientReflectances;
    vec3 diffuseReflectances;
    vec3 specularReflectances;
    float specularExponent;
};

uniform Material uMaterial;
uniform Light uLight;
uniform samplerCube uShadowCubeMap;
uniform vec4 uHighlightColor;

// Functions

vec3 ambientColor() {
    return uMaterial.ambientReflectances * uLight.color;
}

vec3 diffuseColor(in vec3 N, in vec3 L) {
    float cosTheta = max(dot(N, L), 0.0);
    return uMaterial.diffuseReflectances * uLight.color * cosTheta;
}

vec3 specularColor(in vec3 N, in vec3 L, in vec3 V) {
    vec3 H = normalize(oToLight + oToCamera);
    float specularContribution = pow(max(dot(N, H), 0.0), uMaterial.specularExponent);
    return uMaterial.specularReflectances * uLight.color * specularContribution;
}

float isInShadow()
{
    vec3 lightToFrag = oFragWorldPos.xyz - uLight.position;
    // This is in [0, 1] range
    float closestDepth = texture(uShadowCubeMap, lightToFrag).r;
    // Back to real distances
    closestDepth *= uLight.farClipPlane;
    float currentDepth = length(lightToFrag);
    float bias = 0.05;
    return currentDepth - bias > closestDepth ? 1.0 : 0.0;
}

void main() {
    vec3 L = normalize(oToLight);
    vec3 V = normalize(oToCamera);
    vec3 N = normalize(oNormal);
    
    vec3 ambientColor = ambientColor();
    vec3 diffuseColor = diffuseColor(N, L);
    vec3 specularColor = specularColor(N, L, V);

    outputFragColor = vec4((ambientColor + (1.0 - isInShadow()) * (diffuseColor + specularColor)), 1.0) * texture(uMaterial.diffuseTexture, oTexCoords.st) + uHighlightColor;
}
