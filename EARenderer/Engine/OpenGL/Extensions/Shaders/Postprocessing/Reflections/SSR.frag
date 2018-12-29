#version 400 core

#include "Packing.glsl"
#include "GBuffer.glsl"
#include "CameraUBO.glsl"

// Output

layout(location = 0) out vec4 oRayHitInfo;

// Input

in vec2 vTexCoords;

// Uniforms

uniform sampler2D uFrame;

uniform usampler2D uMaterialData;
uniform sampler2D uGBufferHiZBuffer;
uniform int uHiZBufferMipCount;

uniform vec2 uCameraNearFarPlanes;
uniform vec3 uCameraPosition;
uniform mat4 uCameraViewMat;
uniform mat4 uCameraProjectionMat;
uniform mat4 uCameraViewInverse;
uniform mat4 uCameraProjectionInverse;

// Types
struct RayHit {
    float attenuationFactor;
    vec3 ssHitPosition;
};

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
    uvec4 materialData = texture(uMaterialData, raySample.xy);
    vec3 reflectionNormal = DecodeGBufferCookTorranceNormal(materialData);
    return smoothstep(-0.17, 0.0, dot(reflectionNormal, -worldReflectionVec));
}

bool RayMarch(vec3 worldReflectionVec,
              vec3 screenSpaceReflectionVec,
              vec3 screenSpacePos,
              out vec3 hitPosition,
              out float attenuationFactor)
{
    const float kMaxRayMarchStep = 0.04;
    const int kMaxRayMarchIterations = 25;
    const int kMaxBinarySearchSamples = 8;

    int stub = uHiZBufferMipCount;
    bool bFoundIntersection = false;
    vec3 minraySample = vec3(0.0);
    vec3 maxraySample = vec3(0.0);

    float viewportAttenuationFactor = 1.0;

    // Raymarch in the direction of the ScreenSpaceReflectionVec until you get an intersection with your z buffer
    for (int rayStepIdx = 0; rayStepIdx < kMaxRayMarchIterations; rayStepIdx++) {

        vec3 raySample = float(rayStepIdx) * kMaxRayMarchStep * screenSpaceReflectionVec + screenSpacePos;

        if (IsSamplingOutsideViewport(raySample, viewportAttenuationFactor)) {
            attenuationFactor = 0.0;
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

    // Binary search
    if (bFoundIntersection) {

        vec3 midraySample;
        for (int i = 0; i < kMaxBinarySearchSamples; i++) {
            midraySample = mix(minraySample, maxraySample, 0.5);
            float ZBufferVal = texture(uGBufferHiZBuffer, midraySample.xy).r;

            if (midraySample.z > ZBufferVal) {
                maxraySample = midraySample;
            } else {
                minraySample = midraySample;
            }
        }

        float backFaceAttenuationFactor = BackFaceAttenuation(midraySample, worldReflectionVec);
        attenuationFactor = viewportAttenuationFactor * backFaceAttenuationFactor;
        hitPosition = midraySample;
    }

    return bFoundIntersection;
}

RayHit ScreenSpaceReflection(vec3 N, vec3 worldPosition) {
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
        return RayHit(0.0, vec3(0.0));
    }

    // Compute second screen space point so that we can get the SS reflection vector
    vec4 pointAlongReflectionVec = vec4(10.0 * reflectionWorldVec + worldPosition, 1.0);
    vec4 screenSpaceReflectionPoint = uCameraProjectionMat * uCameraViewMat * pointAlongReflectionVec;
    screenSpaceReflectionPoint /= screenSpaceReflectionPoint.w;
    screenSpaceReflectionPoint.xyz = screenSpaceReflectionPoint.xyz * 0.5 + 0.5; // To [0; 1]

    // Compute the sreen space reflection vector as the difference of the two screen space points
    vec3 screenSpaceReflectionVec = normalize(screenSpaceReflectionPoint.xyz - screenSpacePos.xyz);

    vec3 outHitPosition = vec3(0.0);
    float outAttenuationFactor = 0.0;

    bool rayHitDetected = RayMarch(reflectionWorldVec, screenSpaceReflectionVec.xyz, screenSpacePos.xyz, outHitPosition, outAttenuationFactor);

    return RayHit(outAttenuationFactor, outHitPosition);
}

////////////////////////////////////////////////////////////
////////////////////////// Main ////////////////////////////
////////////////////////////////////////////////////////////

void main() {
    uvec4 materialData = texture(uMaterialData, vTexCoords);
    GBufferCookTorrance gBuffer = DecodeGBufferCookTorrance(materialData);

    vec3 worldPosition  = ReconstructWorldPosition(uGBufferHiZBuffer, vTexCoords, uCameraViewInverse, uCameraProjectionInverse);
    vec3 N              = gBuffer.normal;

    RayHit SSR = ScreenSpaceReflection(N, worldPosition);
    oRayHitInfo = vec4(SSR.ssHitPosition, SSR.attenuationFactor);
}
