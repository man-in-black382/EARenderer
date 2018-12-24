#version 410 core

#include "Packing.glsl"
#include "Constants.glsl"
#include "CookTorranceMaterialOverridesUBO.glsl"

// Output

layout(location = 0) out uvec4 oMaterialData;
layout(location = 1) out float oDepth;

// Input

in vec3 vTexCoords;
in vec3 vWorldPosition;
in mat3 vTBN;
in vec3 vPosInTangentSpace;
in vec3 vCameraPosInTangentSpace;

// Uniforms

struct MaterialCookTorrance {
    sampler2D albedoMap;
    sampler2D normalMap;
    sampler2D metallicMap;
    sampler2D roughnessMap;
    sampler2D AOMap; // Ambient occlusion
    sampler2D displacementMap; // Parallax occlusion displacements
};

struct MaterialEmissive {
    vec3 emission; // Emission color
};

uniform MaterialCookTorrance uMaterialCookTorrance;
uniform MaterialEmissive uMaterialEmissive;
uniform int uMaterialType;

// Functions

////////////////////////////////////////////////////////////
///////////////////////// Helpers //////////////////////////
////////////////////////////////////////////////////////////

vec3 LinearFromSRGB(vec3 sRGB) {
    return pow(sRGB, vec3(2.2));
}

vec3 FetchAlbedoMap(vec2 texCoords) {
    return LinearFromSRGB(texture(uMaterialCookTorrance.albedoMap, texCoords).rgb);
}

vec3 FetchNormalMap(vec2 texCoords) {
    vec3 normal = texture(uMaterialCookTorrance.normalMap, texCoords).xyz;
    return normalize(vTBN * (normal * 2.0 - 1.0));
}

float FetchMetallicMap(vec2 texCoords) {
    return texture(uMaterialCookTorrance.metallicMap, texCoords).r;
}

float FetchRoughnessMap(vec2 texCoords) {
    return texture(uMaterialCookTorrance.roughnessMap, texCoords).r;
}

float FetchAOMap(vec2 texCoords) {
    return texture(uMaterialCookTorrance.AOMap, texCoords).r;
}

float FetchDisplacementMap() {
    return texture(uMaterialCookTorrance.displacementMap, vTexCoords.st).r;
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
    float currentDepthMapValue = 1.0 - texture(uMaterialCookTorrance.displacementMap, currentTexCoords).r;

    while(currentLayerDepth < currentDepthMapValue) {
        // Shift texture coordinates along direction of P
        currentTexCoords -= deltaTexCoords;
        // Get depthmap value at current texture coordinates
        currentDepthMapValue = 1.0 - texture(uMaterialCookTorrance.displacementMap, currentTexCoords).r;
        // Get depth of next layer
        currentLayerDepth += layerDepth;
    }

    // Get texture coordinates before collision (reverse operations)
    vec2 prevTexCoords = currentTexCoords + deltaTexCoords;

    // Get depth after and before collision for linear interpolation
    float afterDepth  = currentDepthMapValue - currentLayerDepth;
    float beforeDepth = (1.0 - texture(uMaterialCookTorrance.displacementMap, prevTexCoords).r) - currentLayerDepth + layerDepth;

    // Interpolation of texture coordinates
    float weight = afterDepth / (afterDepth - beforeDepth);
    vec2 finalTexCoords = prevTexCoords * weight + currentTexCoords * (1.0 - weight);

    return finalTexCoords;
}

////////////////////////////////////////////////////////////
////////////////////////// Main ////////////////////////////
////////////////////////////////////////////////////////////

void EncodeCookTorranceMaterial() {
    // Packing scheme
    //
    // | Albedo R | Albedo G | Albedo B | Rougness | Metalness |    AO    |        Normal Z      |
    // |          |          |          |          |           |          |                      |
    // |  1 Byte  |  1 Byte  |  1 Byte  |  1 Byte  |  1 Byte   |  1 Byte  |        2 Bytes       |
    // |__________|__________|__________|__________|___________|__________|______________________|
    // |______First component of output UVEC4______|_______Second component of output UVEC4______|
    //
    //
    // |        Normal X      |        Normal Y      |                  Material type              |
    // |                      |                      |                                             |
    // |        2 Bytes       |        2 Bytes       |                    4 bytes                  |
    // |______________________|______________________|_____________________________________________|
    // |________Third component of output UVEC4______|_______Fourth component of output UVEC4______|

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

    oMaterialData = uvec4(albedoRoughness, metalnessAONormalZ, normalXY, uint(MaterialTypeCookTorrance));
}

void EncodeEmissiveMaterial() {
    uvec3 emission = floatBitsToUint(uMaterialEmissive.emission);
    oMaterialData = uvec4(emission, uint(MaterialTypeEmissive));
}

void main() {
    switch (uMaterialType) {
        case 0: EncodeCookTorranceMaterial(); break;
        case 1: EncodeEmissiveMaterial(); break;
    }

    oDepth = gl_FragCoord.z;
}
