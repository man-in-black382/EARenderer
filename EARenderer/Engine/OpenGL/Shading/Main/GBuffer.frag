#version 400 core

// Constants

const float PI = 3.1415926535897932384626433832795;

// Output

layout(location = 0) out vec4 oOutputData;
layout(location = 0) out vec4 oAlbedoAndRoughness;
layout(location = 1) out vec4 oNormalAndMetalness;
layout(location = 2) out vec4 oWorldPositionAndAO;

// Input

in vec3 vTexCoords;
in vec3 vWorldPosition;
in mat3 vTBN;

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

float Encode8888(vec4 vector) {
    uint rgba = (uint(vector.x * 255.0) << 24) |
                (uint(vector.y * 255.0) << 16) |
                (uint(vector.z * 255.0) << 8) |
                 uint(vector.w * 255.0);
    return uintBitsToFloat(rgba);
}

////////////////////////////////////////////////////////////
////////////////////////// Main ////////////////////////////
////////////////////////////////////////////////////////////

void main() {
    float roughness = FetchRoughnessMap(vTexCoords.st);
    float metallic  = FetchMetallicMap(vTexCoords.st);
    float ao        = FetchAOMap(vTexCoords.st);
    vec3 albedo     = FetchAlbedoMap(vTexCoords.st);
    vec3 N          = FetchNormalMap(vTexCoords.st);

//    oOutputData.r = Encode8888(vec4(albedo, roughness));

    oAlbedoAndRoughness = vec4(albedo, roughness);
    oNormalAndMetalness = vec4(N, metallic);
    oWorldPositionAndAO = vec4(vWorldPosition, ao);
}
