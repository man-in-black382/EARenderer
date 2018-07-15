#version 400 core

// Constants

const float PI = 3.1415926535897932384626433832795;

// Output

layout(location = 0) out uvec3 oAlbedoRoughnessMetalnessAONormal;
layout(location = 1) out float oDepth;

// Input

in vec3 vTexCoords;
in vec3 vWorldPosition;
in mat3 vTBN;
in float vZ;

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

uniform vec2 uCameraNearFarPlanes;

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

uint Encode8888(vec4 vector) {
    uint rgba = (uint(vector.x * 255.0) << 24) |
                (uint(vector.y * 255.0) << 16) |
                (uint(vector.z * 255.0) << 8) |
                 uint(vector.w * 255.0);
    return rgba;
}

uint PackSnorm2x16(float first, float second, float range) {
    const float base = 32767.0;

    uint packed = 0;
    uint iFirst = uint(int(first / range * base));
    uint iSecond = uint(int(second / range * base));

    uint firstSignMask = iFirst & (1u << 31); // Leave only sign bit

    uint secondSignMask = iSecond & (1u << 31); // Leave only sign bit
    secondSignMask >>= 16; // Move sign mask by 16 since second value will be stored in LSB of the final uint

    // Move uFirst into MS bits
    packed |= iFirst;
    packed <<= 16;
    packed |= firstSignMask; // Set sign bit

    // Move uSecond into LS bits
    packed |= iSecond & 0x0000FFFFu;
    packed |= secondSignMask; // Set sign bit

    return packed;
}

float LinearDepth() {
    float near = uCameraNearFarPlanes.x;
    float far = uCameraNearFarPlanes.y;

    float z = gl_FragCoord.z * 2.0 - 1.0; // [-1; 1]

    float viewSpaceLinearDepth = (2.0 * near * far) / (far + near - z * (far - near));
    float normalizedLinearDepth = viewSpaceLinearDepth / (far - near);

    return normalizedLinearDepth;

    // REMINDER: Opengl is LEFT-HANDED (positive Z pointing into the screen)
    // Camera is RIGHT-HANDED (positive Z pointing FROM the screen into your face!) therefore
    // camera view space depth (Z) values are NEGATIVE

//    return vZ;
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

    float roughness = FetchRoughnessMap(vTexCoords.st);
    float metallic  = FetchMetallicMap(vTexCoords.st);
    float ao        = FetchAOMap(vTexCoords.st);
    vec3 albedo     = FetchAlbedoMap(vTexCoords.st);
    vec3 N          = FetchNormalMap(vTexCoords.st);

    uint albedoRoughness = Encode8888(vec4(albedo, roughness));
    uint metalnessAO     = Encode8888(vec4(metallic, ao, 0.0, 0.0)); // Metalness and AO are packed to MSB

    uint normalZ  = PackSnorm2x16(0.0, N.z, 1.0); // Pack Z to LSB
    uint normalXY = PackSnorm2x16(N.x, N.y, 1.0);

    uint metalnessAONormalZ = metalnessAO | normalZ;

    oAlbedoRoughnessMetalnessAONormal = uvec3(albedoRoughness, metalnessAONormalZ, normalXY);
    oDepth = LinearDepth();
}
