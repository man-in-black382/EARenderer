#version 400

out vec4 outputFragColor;

in vec3 oNormal;
in vec3 oToLight;
in vec3 oToCamera;
in vec3 oTexCoords;

uniform vec3 uLightColor;

struct Material {
    sampler2D diffuseTexture;
    vec3 ambientReflectances;
    vec3 diffuseReflectances;
    vec3 specularReflectances;
    float specularExponent;
};

uniform Material uMaterial;
 
vec3 ambientColor() {
    return uMaterial.ambientReflectances * uLightColor;
}

vec3 diffuseColor(in vec3 N, in vec3 L) {
    float cosTheta = max(dot(N, L), 0.0);
    return uMaterial.diffuseReflectances * uLightColor * cosTheta;
}

vec3 specularColor(in vec3 N, in vec3 L, in vec3 V) {
    vec3 H = normalize(oToLight + oToCamera);
    float specularContribution = pow(max(dot(N, H), 0.0), uMaterial.specularExponent);
    return uMaterial.specularReflectances * uLightColor * specularContribution;
}

void main() {
    vec3 L = normalize(oToLight);
    vec3 V = normalize(oToCamera);
    vec3 N = normalize(oNormal);
    
    vec3 ambientColor = ambientColor();
    vec3 diffuseColor = diffuseColor(N, L);
    vec3 specularColor = specularColor(N, L, V);
    
    outputFragColor = vec4((ambientColor + diffuseColor + specularColor), 1.0) * texture(uMaterial.diffuseTexture, oTexCoords.st);
}
