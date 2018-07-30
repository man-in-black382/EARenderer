#version 400 core

// Output

layout(location = 0) out vec4 oOutputColor;

// Input

in vec2 vTexCoords;

// Uniforms


uniform sampler2D uFrame;

uniform usampler2D uGBufferAlbedoRoughnessMetalnessAONormal;
uniform sampler2D uGBufferHiZBuffer;
uniform int uHiZBufferMipCount;

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
    float depth = texture(uGBufferHiZBuffer, vTexCoords).r;

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

// Autodesk tracing

#define HIZ_START_LEVEL 6
#define HIZ_STOP_LEVEL 0
#define HIZ_MAX_LEVEL 7
#define MAX_ITERATIONS 100

vec2 cell(vec2 ray, vec2 cell_count, uint camera) {
    return floor(ray.xy * cell_count);
}

vec2 cell_count(float level) {
    vec2 texSize = vec2(textureSize(uGBufferHiZBuffer, 0));
    return texSize / (level == 0.0 ? 1.0 : exp2(level));
//    return input_texture2_size / (level == 0.0 ? 1.0 : exp2(level));
}

vec3 intersect_cell_boundary(vec3 pos, vec3 dir, vec2 cell_id, vec2 cell_count, vec2 cross_step, vec2 cross_offset, uint camera) {
    vec2 cell_size = 1.0 / cell_count;
    vec2 planes = cell_id/cell_count + cell_size * cross_step;

    vec2 solutions = (planes - pos.xy)/dir.xy;
    vec3 intersection_pos = pos + dir * min(solutions.x, solutions.y);

    intersection_pos.xy += (solutions.x < solutions.y) ? vec2(cross_offset.x, 0.0) : vec2(0.0, cross_offset.y);

    return intersection_pos;
}

bool crossed_cell_boundary(vec2 cell_id_one, vec2 cell_id_two) {
    return int(cell_id_one.x) != int(cell_id_two.x) || int(cell_id_one.y) != int(cell_id_two.y);
}

float minimum_depth_plane(vec2 ray, float level, vec2 cell_count, uint camera) {
    return textureLod(uGBufferHiZBuffer, ray, level).r;
//    return input_texture2.Load(int3(vr_stereo_to_mono(ray.xy, camera) * cell_count, level)).r;
}

vec3 hi_z_trace(vec3 p, vec3 v, in uint camera, out uint iterations) {
    float level = HIZ_START_LEVEL;
    vec3 v_z = v/v.z;
    vec2 hi_z_size = cell_count(level);
    vec3 ray = p;

    vec2 cross_step = vec2(v.x >= 0.0 ? 1.0 : -1.0, v.y >= 0.0 ? 1.0 : -1.0);
    vec2 cross_offset = cross_step * 0.00001;
    cross_step = clamp(cross_step, 0.0, 1.0);

    vec2 ray_cell = cell(ray.xy, hi_z_size.xy, camera);
    ray = intersect_cell_boundary(ray, v, ray_cell, hi_z_size, cross_step, cross_offset, camera);

    iterations = 0;
    while(level >= HIZ_STOP_LEVEL && iterations < MAX_ITERATIONS) {
        // get the cell number of the current ray
        vec2 current_cell_count = cell_count(level);
        vec2 old_cell_id = cell(ray.xy, current_cell_count, camera);

        // get the minimum depth plane in which the current ray resides
        float min_z = minimum_depth_plane(ray.xy, level, current_cell_count, camera);

        // intersect only if ray depth is below the minimum depth plane
        vec3 tmp_ray = ray;
        if(v.z > 0) {
            float min_minus_ray = min_z - ray.z;
            tmp_ray = min_minus_ray > 0 ? ray + v_z*min_minus_ray : tmp_ray;
            vec2 new_cell_id = cell(tmp_ray.xy, current_cell_count, camera);
            if(crossed_cell_boundary(old_cell_id, new_cell_id)) {
                tmp_ray = intersect_cell_boundary(ray, v, old_cell_id, current_cell_count, cross_step, cross_offset, camera);
                level = min(HIZ_MAX_LEVEL, level + 2.0f);
            }else{
                if(level == 1 && abs(min_minus_ray) > 0.0001) {
                    tmp_ray = intersect_cell_boundary(ray, v, old_cell_id, current_cell_count, cross_step, cross_offset, camera);
                    level = 2;
                }
            }
        } else if(ray.z < min_z) {
            tmp_ray = intersect_cell_boundary(ray, v, old_cell_id, current_cell_count, cross_step, cross_offset, camera);
            level = min(HIZ_MAX_LEVEL, level + 2.0f);
        }

        ray.xyz = tmp_ray.xyz;
        --level;

        ++iterations;
    }
    return ray;
}

// Thanks to Sakib Saikia
// and his post https://sakibsaikia.github.io/graphics/2016/12/25/Screen-Space-Reflection-in-Killing-Floor-2.html#fn:fn3

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

#define MAX_REFLECTION_RAY_MARCH_STEP 0.05
#define NUM_RAY_MARCH_SAMPLES 50
#define NUM_BINARY_SEARCH_SAMPLES 5

bool GetReflection(vec3 worldReflectionVec,
                   vec3 screenSpaceReflectionVec,
                   vec3 screenSpacePos,
                   out vec3 reflectionColor)
{
    int stub = uHiZBufferMipCount;
    bool bFoundIntersection = false;
    vec3 minraySample = vec3(0.0);
    vec3 maxraySample = vec3(0.0);

    float viewportAttenuationFactor = 1.0;

    // Raymarch in the direction of the ScreenSpaceReflectionVec until you get an intersection with your z buffer
    for (int rayStepIdx = 0; rayStepIdx < NUM_RAY_MARCH_SAMPLES; rayStepIdx++) {

        vec3 raySample = (float(rayStepIdx) * MAX_REFLECTION_RAY_MARCH_STEP) * screenSpaceReflectionVec + screenSpacePos;

        if (IsSamplingOutsideViewport(raySample, viewportAttenuationFactor)) {
            reflectionColor = vec3(0.5, 0.5, 0.0);
            return false;
        }

        float ZBufferVal = texture(uGBufferHiZBuffer, raySample.xy).r;

        maxraySample = raySample;

        float bias = rayStepIdx == 0 ? 0.001 : 0.0;

        if (raySample.z > ZBufferVal + bias) {
            bFoundIntersection = true;
            break;
        }

        minraySample = maxraySample;
    }

    if (bFoundIntersection) {

        vec3 midraySample;
        for (int i = 0; i < NUM_BINARY_SEARCH_SAMPLES; i++)
        {
            midraySample = mix(minraySample, maxraySample, 0.5);
            float ZBufferVal = texture(uGBufferHiZBuffer, midraySample.xy).r;

            if (midraySample.z > ZBufferVal) {
                maxraySample = midraySample;
            } else {
                minraySample = midraySample;
            }
        }

        reflectionColor = texture(uFrame, midraySample.xy).rgb;

        uvec3 albedoRoughnessMetalnessAONormal = texture(uGBufferAlbedoRoughnessMetalnessAONormal, midraySample.xy).xyz;
        vec4 albedoRoughness = Decode8888(albedoRoughnessMetalnessAONormal.x);
        reflectionColor = albedoRoughness.rgb;

        float backFaceAttenuationFactor = BackFaceAttenuation(midraySample, worldReflectionVec);
        reflectionColor *= viewportAttenuationFactor * backFaceAttenuationFactor;
    }

    return bFoundIntersection;
}

vec3 ScreenSpaceReflection(vec3 N, vec3 worldPosition) {
    vec2 currentFragUV = vTexCoords;

    // Prerequisites
    float fragDepth = texture(uGBufferHiZBuffer, currentFragUV).r;
    vec3 cameraToFrag = normalize(worldPosition - uCameraPosition);

    // ScreenSpacePos --> (screencoord.xy, device_z)
    vec4 screenSpacePos = vec4(currentFragUV, fragDepth, 1.0);

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
    screenSpaceReflectionPoint.xyz = screenSpaceReflectionPoint.xyz * 0.5 + 0.5; // To [0; 1]

    // Compute the sreen space reflection vector as the difference of the two screen space points
    vec3 screenSpaceReflectionVec = normalize(screenSpaceReflectionPoint.xyz - screenSpacePos.xyz);

    vec3 outReflectionColor;
    if (GetReflection(reflectionWorldVec, screenSpaceReflectionVec.xyz, screenSpacePos.xyz, outReflectionColor)) {
        return outReflectionColor * attenuationFactor;
    } else {
        return outReflectionColor;
    }
}

////////////////////////////////////////////////////////////
////////////////////// Cone Tracing ////////////////////////
////////////////////////////////////////////////////////////

float SpecularPowerToConeAngle(float specularPower) {
    // based on phong distribution model
    const float kMaxSpecExp = 16.0;
    if(specularPower >= exp2(kMaxSpecExp)) {
        return 0.0f;
    }
    const float xi = 0.244f;
    float exponent = 1.0f / (specularPower + 1.0f);
    return acos(pow(xi, exponent));
}

float IsoscelesTriangleOpposite(float adjacentLength, float coneTheta) {
    return 2.0f * tan(coneTheta) * adjacentLength;
}

// a - opposite length of the triangle
// h - adjacent length of the triangle
float IsoscelesTriangleInscribedCircleRadius(float a, float h) {
    float a2 = a * a;
    float fh2 = 4.0f * h * h;
    return (a * (sqrt(a2 + fh2) - a)) / (4.0f * h);
}

//vec4 ConeSampleWeightedColor(float2 samplePos, float mipChannel, float gloss)
//{
//    vec3 sampleColor = colorBuffer.SampleLevel(sampTrilinearClamp, samplePos, mipChannel).rgb;
//    return float4(sampleColor * gloss, gloss);
//}

float IsoscelesTriangleNextAdjacent(float adjacentLength, float incircleRadius)
{
    // subtract the diameter of the incircle to get the adjacent side of the next level on the cone
    return adjacentLength - (incircleRadius * 2.0f);
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
