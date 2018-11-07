#version 400 core

#include "Packing.glsl"

// Constants

const float PI = 3.1415926535897932384626433832795;

// Output

layout(location = 0) out uvec3 oAlbedoRoughnessMetalnessAONormal;
layout(location = 1) out float oDepth;

// Input

in vec3 vTexCoords;
in vec3 vWorldPosition;
in mat3 vTBN;
in vec3 vPosInTangentSpace;
in vec3 vCameraPosInTangentSpace;

// Uniforms

struct Material {
    sampler2D albedoMap;
    sampler2D normalMap;
    sampler2D metallicMap;
    sampler2D roughnessMap;
    sampler2D AOMap; // Ambient occlusion
    sampler2D displacementMap; // Parallax occlusion displacements
};

uniform Material uMaterial;

// Functions

////////////////////////////////////////////////////////////
///////////////////////// Helpers //////////////////////////
////////////////////////////////////////////////////////////

vec3 LinearFromSRGB(vec3 sRGB) {
    return pow(sRGB, vec3(2.2));
}

vec3 FetchAlbedoMap(vec2 texCoords) {
    return LinearFromSRGB(texture(uMaterial.albedoMap, texCoords).rgb);
}

vec3 FetchNormalMap(vec2 texCoords) {
    vec3 normal = texture(uMaterial.normalMap, texCoords).xyz;
    return normalize(vTBN * (normal * 2.0 - 1.0));
}

float FetchMetallicMap(vec2 texCoords) {
    return texture(uMaterial.metallicMap, texCoords).r;
}

float FetchRoughnessMap(vec2 texCoords) {
    return texture(uMaterial.roughnessMap, texCoords).r;
}

float FetchAOMap(vec2 texCoords) {
    return texture(uMaterial.AOMap, texCoords).r;
}

float FetchDisplacementMap() {
    return texture(uMaterial.displacementMap, vTexCoords.st).r;
}

vec2 DisplacedTextureCoords() {

    float uParallaxMappingStrength = 0.015;

    vec2 texCoords = vTexCoords.st;
    vec3 viewDir = normalize(vCameraPosInTangentSpace - vPosInTangentSpace);

    // Number of depth layers
    const float minLayers = 8;
    const float maxLayers = 32;
    float numLayers = mix(maxLayers, minLayers, abs(dot(vec3(0.0, 0.0, 1.0), viewDir)));
    // Calculate the size of each layer
    float layerDepth = 1.0 / numLayers;
    // Depth of current layer
    float currentLayerDepth = 0.0;
    // The amount to shift the texture coordinates per layer (from vector P)
    vec2 P = viewDir.xy / viewDir.z * uParallaxMappingStrength;

    vec2 deltaTexCoords = P / numLayers;

    // Get initial values
    vec2  currentTexCoords     = texCoords;
    float currentDepthMapValue = 1.0 - texture(uMaterial.displacementMap, currentTexCoords).r;

    while(currentLayerDepth < currentDepthMapValue) {
        // Shift texture coordinates along direction of P
        currentTexCoords -= deltaTexCoords;
        // Get depthmap value at current texture coordinates
        currentDepthMapValue = 1.0 - texture(uMaterial.displacementMap, currentTexCoords).r;
        // Get depth of next layer
        currentLayerDepth += layerDepth;
    }

    // Get texture coordinates before collision (reverse operations)
    vec2 prevTexCoords = currentTexCoords + deltaTexCoords;

    // Get depth after and before collision for linear interpolation
    float afterDepth  = currentDepthMapValue - currentLayerDepth;
    float beforeDepth = (1.0 - texture(uMaterial.displacementMap, prevTexCoords).r) - currentLayerDepth + layerDepth;

    // Interpolation of texture coordinates
    float weight = afterDepth / (afterDepth - beforeDepth);
    vec2 finalTexCoords = prevTexCoords * weight + currentTexCoords * (1.0 - weight);

    return finalTexCoords;
}

////////////////////////////////////////////////////////////
////////////////////////// Main ////////////////////////////
////////////////////////////////////////////////////////////

void main() {
    // Packing scheme
    //
    // | Albedo R | Albedo G | Albedo B | Rougness | Metalness |    AO    |        Normal Z      |
    // |          |          |          |          |           |          |                      |
    // |  1 Byte  |  1 Byte  |  1 Byte  |  1 Byte  |  1 Byte   |  1 Byte  |        2 Bytes       |
    // |__________|__________|__________|__________|___________|__________|______________________|
    // |______First component of output UVEC3______|_______Second component of output UVEC3______|
    //
    //
    // |        Normal X      |        Normal Y      |
    // |                      |                      |
    // |        2 Bytes       |        2 Bytes       |
    // |______________________|______________________|
    // |________Third component of output UVEC3______|

    vec2 texCoords = vTexCoords.st; //DisplacedTextureCoords();

    float roughness = FetchRoughnessMap(texCoords);
    float metallic  = FetchMetallicMap(texCoords);
    float ao        = FetchAOMap(texCoords);
    vec3 albedo     = FetchAlbedoMap(texCoords);
    vec3 N          = FetchNormalMap(texCoords);

    uint albedoRoughness = Encode8888(vec4(albedo, roughness));
    uint metalnessAO     = Encode8888(vec4(metallic, ao, 0.0, 0.0)); // Metalness and AO are packed to MSB

    uint normalZ  = PackSnorm2x16(0.0, N.z, 1.0); // Pack Z to LSB
    uint normalXY = PackSnorm2x16(N.x, N.y, 1.0);

    uint metalnessAONormalZ = metalnessAO | normalZ;

    oAlbedoRoughnessMetalnessAONormal = uvec3(albedoRoughness, metalnessAONormalZ, normalXY);
    oDepth = gl_FragCoord.z;
}
