#include "Packing.glsl"

struct GBufferCookTorrance {
    vec3 albedo;
    vec3 normal;
    float roughness;
    float metalness;
    float AO;
};

struct GBufferEmissive {
    vec3 emission;
};

// GBufferCookTorrance packing scheme
//
// | Albedo R | Albedo G | Albedo B | Roughness | Metalness |    AO    |        Normal Z      |
// |          |          |          |           |           |          |                      |
// |  1 Byte  |  1 Byte  |  1 Byte  |   1 Byte  |  1 Byte   |  1 Byte  |        2 Bytes       |
// |__________|__________|__________|___________|___________|__________|______________________|
// |______First component of output UVEC3_______|_______Second component of output UVEC3______|
//
//
// |        Normal X      |        Normal Y      |
// |                      |                      |
// |        2 Bytes       |        2 Bytes       |
// |______________________|______________________|
// |________Third component of output UVEC3______|
//
GBufferCookTorrance DecodeGBufferCookTorrance(uvec4 materialData) {
     GBufferCookTorrance gBuffer;

     uvec3 encoded = materialData.xyz;
     vec4 albedoRoughness = Decode8888(encoded.x);
     uint metalnessAONormalZ = encoded.y;
     vec2 metalnessAO = Decode8888(metalnessAONormalZ).xy;
     float normalZ = UnpackSnorm2x16(metalnessAONormalZ, 1.0).y;
     vec2 normalXY = UnpackSnorm2x16(encoded.z, 1.0);

     gBuffer.albedo    = albedoRoughness.rgb;
     gBuffer.normal    = vec3(normalXY, normalZ);
     gBuffer.roughness = albedoRoughness.a;
     gBuffer.metalness = metalnessAO.r;
     gBuffer.AO        = metalnessAO.g;

     return gBuffer;
 }

 GBufferEmissive DecodeGBufferEmissive(uvec4 materialData) {
     GBufferEmissive gBuffer;
     uvec3 encoded = materialData.xyz;
     gBuffer.emission = uintBitsToFloat(encoded);
     return gBuffer;
 }

 int DecodeGBufferMaterialType(uvec4 materialData) {
     return int(materialData.w);
 }

vec3 ReconstructWorldPosition(float hyperbolicDepth, // Depth value after viewport transformation in [0; 1] range
                              vec2 normTexCoords, // Normalized texture coordinates [0; 1]
                              mat4 inverseView, // Inverse camera view matrix
                              mat4 inverseProjection) // Inverse camera projection matrix
{
    // Depth range in NDC is [-1; 1]
    // Default value for glDepthRange is [-1; 1]
    // OpenGL uses values from glDepthRange to transform depth to [0; 1] range during viewport transformation
    // To reconstruct world position using inverse camera matrices we need depth in [-1; 1] range again
    float z = hyperbolicDepth * 2.0 - 1.0;
    vec2 xy = normTexCoords * 2.0 - 1.0;

    vec4 clipSpacePosition = vec4(xy, z, 1.0);
    vec4 viewSpacePosition = inverseProjection * clipSpacePosition;

    // Perspective division
    viewSpacePosition /= viewSpacePosition.w;

    vec4 worldSpacePosition = inverseView * viewSpacePosition;

    return worldSpacePosition.xyz;
}

vec3 ReconstructWorldPosition(sampler2D depthSampler, vec2 normTexCoords, mat4 inverseView, mat4 inverseProjection) {
    float depth = texture(depthSampler, normTexCoords).r;
    return ReconstructWorldPosition(depth, normTexCoords, inverseView, inverseProjection);
}

vec3 DecodeGBufferCookTorranceNormal(uvec4 materialData) {
    uvec3 encoded = materialData.xyz;
    uint metalnessAONormalZ = encoded.y;
    float normalZ = UnpackSnorm2x16(metalnessAONormalZ, 1.0).y;
    vec2 normalXY = UnpackSnorm2x16(encoded.z, 1.0);
    return vec3(normalXY, normalZ);
}
//
//// Input is linear [0; 1] depth value
//// Output is [-1; 1] non-linear depth value ready to be used in WP reconstruction
//float NDCHyperbolicDepthFromLinearDepth(float linearDepth) {
//    float zNear = uCameraNearFarPlanes.x;
//    float zFar = uCameraNearFarPlanes.y;
//    float nonLinearDepth = (zFar + zNear - 2.0 * zNear * zFar / linearDepth) / (zFar - zNear);
//    return nonLinearDepth;
//}
