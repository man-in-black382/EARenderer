#version 400 core

// Output

layout(location = 0) out vec4 oOutputColor;

// Input

in vec2 vTexCoords;

// Uniforms


uniform sampler2D uFrame;

uniform usampler2D uGBufferAlbedoRoughnessMetalnessAONormal;
uniform sampler2D uGBufferLinearDepth;
uniform sampler2D uGBufferHyperbolicDepth;

uniform vec2 uCameraNearFarPlanes;
uniform vec3 uCameraPosition;
uniform mat4 uCameraViewMat;
uniform mat4 uCameraProjectionMat;
uniform mat4 uCameraViewInverse;
uniform mat4 uCameraProjectionInverse;

struct GBuffer {
    vec3 albedo;
    vec3 normal;
    float roughness;
    float metalness;
    float AO;
};

vec2 UnpackSnorm2x16(uint package, float range) {
    const float base = 32767.0;

    // Unpack encoded floats into individual variables
    uint uFirst = package >> 16;
    uint uSecond = package & 0x0000FFFFu;

    // Extract sign bits
    uint firstSignMask = uFirst & (1u << 15);
    uint secondSignMask = uSecond & (1u << 15);

    // If sign bit indicates negativity, fill MS 16 bits with 1s
    uFirst |= firstSignMask != 0 ? 0xFFFF0000u : 0x0u;
    uSecond |= secondSignMask != 0 ? 0xFFFF0000u : 0x0u;

    // Now get signed integer representation
    int iFirst = int(uFirst);
    int iSecond = int(uSecond);

    // At last, convert integers back to floats using range and base
    float fFirst = (float(iFirst) / base) * range;
    float fSecond = (float(iSecond) / base) * range;

    return vec2(fFirst, fSecond);
}

vec4 Decode8888(uint encoded) {
    vec4 decoded;
    decoded.x = (0xFF000000u & encoded) >> 24;
    decoded.y = (0x00FF0000u & encoded) >> 16;
    decoded.z = (0x0000FF00u & encoded) >> 8;
    decoded.w = (0x000000FFu & encoded);
    decoded /= 255.0;
    return decoded;
}

// GBuffer packing scheme
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


vec3 DecodeGBufferNormal(uvec3 gBuffer) {
    uint metalnessAONormalZ = gBuffer.y;
    float normalZ = UnpackSnorm2x16(metalnessAONormalZ, 1.0).y;
    vec2 normalXY = UnpackSnorm2x16(gBuffer.z, 1.0);
    return vec3(normalXY, normalZ);
}

GBuffer DecodeGBuffer() {
    GBuffer gBuffer;

    uvec3 albedoRoughnessMetalnessAONormal = texture(uGBufferAlbedoRoughnessMetalnessAONormal, vTexCoords.st).xyz;
    vec4 albedoRoughness = Decode8888(albedoRoughnessMetalnessAONormal.x);
    uint metalnessAONormalZ = albedoRoughnessMetalnessAONormal.y;
    vec2 metalnessAO = Decode8888(metalnessAONormalZ).xy;
    float normalZ = UnpackSnorm2x16(metalnessAONormalZ, 1.0).y;
    vec2 normalXY = UnpackSnorm2x16(albedoRoughnessMetalnessAONormal.z, 1.0);

    gBuffer.albedo    = albedoRoughness.rgb;
    gBuffer.normal    = vec3(normalXY, normalZ);
    gBuffer.roughness = albedoRoughness.a;
    gBuffer.metalness = metalnessAO.r;
    gBuffer.AO        = metalnessAO.g;

    return gBuffer;
}

vec3 ReconstructWorldPosition() {
    float depth = texture(uGBufferHyperbolicDepth, vTexCoords).r;

    // Depth range in NDC is [-1; 1]
    // Default value for glDepthRange is [-1; 1]
    // OpenGL uses values from glDepthRange to transform depth to [0; 1] range during viewport transformation
    // To reconstruct world position using inverse camera matrices we need depth in [-1; 1] range again
    float z = depth * 2.0 - 1.0;
    vec2 xy = vTexCoords * 2.0 - 1.0;

    vec4 clipSpacePosition = vec4(xy, z, 1.0);
    vec4 viewSpacePosition = uCameraProjectionInverse * clipSpacePosition;

    // Perspective division
    viewSpacePosition /= viewSpacePosition.w;

    vec4 worldSpacePosition = uCameraViewInverse * viewSpacePosition;

    return worldSpacePosition.xyz;
}

////////////////////////////////////////////////////////////
///////////////////////// Reflections  /////////////////////
////////////////////////////////////////////////////////////

// http://imanolfotia.com/blog/update/2017/03/11/ScreenSpaceReflections.html

vec3 Hash(vec3 a) {
    const vec3 scale = vec3(0.8);
    const float K = 19.19;

    a = fract(a * scale);
    a += dot(a, a.yxz + K);

    return fract((a.xxy + a.yxx)*a.zyx);
}

#define MAX_REFLECTION_RAY_MARCH_STEP 0.01
#define NUM_RAY_MARCH_SAMPLES 100

#define NUM_BINARY_SEARCH_SAMPLES 6

// Thanks to Sakib Saikia
// and his post https://sakibsaikia.github.io/graphics/2016/12/25/Screen-Space-Reflection-in-Killing-Floor-2.html#fn:fn3

// Input depth is expected to be in [-1; 1] DNC range
// Outputs [0; 1] linear depth
float LinearizeDepth(float hyperbolicDepth, float nearClipPlane, float farClipPlane) {
    float viewSpaceLinearDepth = (2.0 * nearClipPlane * farClipPlane) / (farClipPlane + nearClipPlane - hyperbolicDepth * (farClipPlane - nearClipPlane));
    float normalizedLinearDepth = viewSpaceLinearDepth / (farClipPlane - nearClipPlane);
    return normalizedLinearDepth;
}

bool IsSamplingOutsideViewport(vec3 raySample, inout float attenuationFactor) {
    // Any rays that march outside the screen viewport will not have any valid pixel information. These need to be dropped.
    vec2 UVSamplingAttenuation = smoothstep(0.0, 0.05, raySample.xy) * (1 - smoothstep(0.95, 1, raySample.xy));
    attenuationFactor = UVSamplingAttenuation.x * UVSamplingAttenuation.y;
    return attenuationFactor <= 0;
}

bool IsReflectedBackToCamera(vec3 reflection, vec3 fragToCamera, inout float attenuationFactor) {
    // This will check the direction of the reflection vector with the view direction,
    // and if they are pointing in the same direction, it will drown out those reflections
    // since we are limited to pixels visible on screen. Attenuate reflections for angles between
    // 60 degrees and 75 degrees, and drop all contribution beyond the (-60,60)  degree range
    attenuationFactor = 1.0 - smoothstep(0.25, 0.5, dot(fragToCamera, reflection));

    // Reject if the reflection vector is pointing back at the viewer.
    return attenuationFactor <= 0;
}

float BackFaceAttenuation(vec3 raySample, vec3 worldReflectionVec) {
    // This will check the direction of the normal of the reflection sample with the
    // direction of the reflection vector, and if they are pointing in the same direction,
    // it will drown out those reflections since backward facing pixels are not available
    // for screen space reflection. Attenuate reflections for angles between 90 degrees
    // and 100 degrees, and drop all contribution beyond the (-100,100)  degree range
    uvec3 albedoRoughnessMetalnessAONormal = texture(uGBufferAlbedoRoughnessMetalnessAONormal, raySample.xy).xyz;
    vec3 reflectionNormal = DecodeGBufferNormal(albedoRoughnessMetalnessAONormal);
    return smoothstep(-0.17, 0.0, dot(reflectionNormal, -worldReflectionVec));
}

bool GetReflection(vec3 worldReflectionVec,
                   vec3 screenSpaceReflectionVec,
                   vec3 screenSpacePos,
                   out vec3 reflectionColor)
{
    bool bFoundIntersection = false;
    vec3 minRaySample = vec3(0.0);
    vec3 maxRaySample = vec3(0.0);

    float viewportAttenuationFactor = 1.0;

    // Raymarch in the direction of the ScreenSpaceReflectionVec until you get an intersection with your z buffer
    for (int rayStepIdx = 0; rayStepIdx < NUM_RAY_MARCH_SAMPLES; rayStepIdx++) {

        vec3 raySample = (float(rayStepIdx) * MAX_REFLECTION_RAY_MARCH_STEP) * screenSpaceReflectionVec + screenSpacePos;

        if (IsSamplingOutsideViewport(raySample, viewportAttenuationFactor)) {
            reflectionColor = vec3(0.5, 0.0, 0.0);
            return false;
        }

        float ZBufferVal = texture(uGBufferLinearDepth, raySample.xy).r;

        maxRaySample = raySample;

        if (raySample.z > ZBufferVal + 0.1) {
            bFoundIntersection = true;
            break;
        }

        minRaySample = maxRaySample;
    }

    if (bFoundIntersection) {

        vec3 midRaySample;
        for (int i = 0; i < NUM_BINARY_SEARCH_SAMPLES; i++)
        {
            midRaySample = mix(minRaySample, maxRaySample, 0.5);
            float ZBufferVal = texture(uGBufferLinearDepth, midRaySample.xy).r;

            if (midRaySample.z > ZBufferVal) {
                maxRaySample = midRaySample;
            } else {
                minRaySample = midRaySample;
            }
        }

        reflectionColor = texture(uFrame, midRaySample.xy).rgb;

        uvec3 albedoRoughnessMetalnessAONormal = texture(uGBufferAlbedoRoughnessMetalnessAONormal, midRaySample.xy).xyz;
        vec4 albedoRoughness = Decode8888(albedoRoughnessMetalnessAONormal.x);
        reflectionColor = albedoRoughness.rgb;

        float backFaceAttenuationFactor = BackFaceAttenuation(midRaySample, worldReflectionVec);
        reflectionColor *= viewportAttenuationFactor * backFaceAttenuationFactor;
    }

    return bFoundIntersection;
}

vec3 ScreenSpaceReflection(vec3 N, vec3 worldPosition) {
    vec2 frameSize = textureSize(uGBufferLinearDepth, 0);
    vec2 currentFragUV = vTexCoords;

    // Prerequisites
    float currentFragLinearDepth = texture(uGBufferLinearDepth, currentFragUV).r;
    vec3 cameraToFrag = normalize(worldPosition - uCameraPosition);

    // ScreenSpacePos --> (screencoord.xy, device_z)
    vec4 screenSpacePos = vec4(currentFragUV, currentFragLinearDepth, 1.0);

    // Compute world space reflection vector
    vec3 reflectionWorldVec = reflect(cameraToFrag, N);

    float attenuationFactor;
    if (IsReflectedBackToCamera(reflectionWorldVec, -cameraToFrag, attenuationFactor)) {
        return vec3(0.5, 0.0, 0.0);
    }

    // Compute second screen space point so that we can get the SS reflection vector
    vec4 pointAlongReflectionVec = vec4(10.0 * reflectionWorldVec + worldPosition, 1.0);
    vec4 screenSpaceReflectionPoint = uCameraProjectionMat * uCameraViewMat * pointAlongReflectionVec;
    screenSpaceReflectionPoint /= screenSpaceReflectionPoint.w;
    screenSpaceReflectionPoint.xy = screenSpaceReflectionPoint.xy * 0.5 + 0.5; // To [0; 1]
    screenSpaceReflectionPoint.z = LinearizeDepth(screenSpaceReflectionPoint.z, uCameraNearFarPlanes.x, uCameraNearFarPlanes.y);

    // Compute the sreen space reflection vector as the difference of the two screen space points
    vec3 screenSpaceReflectionVec = normalize(screenSpaceReflectionPoint.xyz - screenSpacePos.xyz);

    vec3 outReflectionColor;
    if (GetReflection(reflectionWorldVec, screenSpaceReflectionVec.xyz, screenSpacePos.xyz, outReflectionColor)) {
        return outReflectionColor * attenuationFactor;
    } else {
        return vec3(0.5, 0.0, 0.0);
    }
}

////////////////////////////////////////////////////////////
////////////////////////// Main ////////////////////////////
////////////////////////////////////////////////////////////

void main() {
    GBuffer gBuffer     = DecodeGBuffer();

    vec3 worldPosition  = ReconstructWorldPosition();
    float roughness     = gBuffer.roughness;
    float metallic      = gBuffer.metalness;
    float ao            = gBuffer.AO;
    vec3 albedo         = gBuffer.albedo;
    vec3 N              = gBuffer.normal;

    vec3 SSR = ScreenSpaceReflection(N, worldPosition);
    oOutputColor = vec4(SSR, 1.0);
}
