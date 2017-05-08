#version 400

// Output

out vec4 outputFragColor;

// Input

in vec3 oNormal;
in vec3 oToLight;
in vec3 oToCamera;
in vec3 oTexCoords;
in vec4 oPosInLightSpace;

// Uniforms

uniform vec3 uLightColor;
uniform sampler2D uShadowMap;

struct Material {
    sampler2D diffuseTexture;
    vec3 ambientReflectances;
    vec3 diffuseReflectances;
    vec3 specularReflectances;
    float specularExponent;
};

uniform Material uMaterial;

// Functions

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

float isInShadow(in vec3 N, in vec3 L)
{
    // perform perspective divide
    vec3 projCoords = oPosInLightSpace.xyz / oPosInLightSpace.w;
    
    // Transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    
    // Get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(uShadowMap, projCoords.xy).r;
    
    // Get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    
    // Check whether current frag pos is in shadow
    float bias = max(0.05 * (1.0 - dot(N, L)), 0.005);
    return currentDepth - bias > closestDepth ? 1.0 : 0.0;
}

void main() {
    vec3 L = normalize(oToLight);
    vec3 V = normalize(oToCamera);
    vec3 N = normalize(oNormal);
    
    vec3 ambientColor = ambientColor();
    vec3 diffuseColor = diffuseColor(N, L);
    vec3 specularColor = specularColor(N, L, V);
    
    outputFragColor = vec4((ambientColor + (1.0 - isInShadow(N, L)) * (diffuseColor + specularColor)), 1.0) * texture(uMaterial.diffuseTexture, oTexCoords.st);
}
