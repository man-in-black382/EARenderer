#version 400 core

#include "GBuffer.glsl"

// Constants
const float kDenormalizationFactor = 1000.0;

// Output

layout(location = 0) out vec4 oOutputColor;

// Input

in vec2 vTexCoords;

// Uniforms

uniform usampler2D uGBufferAlbedoRoughnessMetalnessAONormal;
uniform sampler2D uGBufferHiZBuffer;
uniform sampler2D uReflections;
uniform sampler2D uRayHitInfo;
uniform int uMipCount;

uniform vec2 uCameraNearFarPlanes;
uniform vec3 uCameraPosition;
uniform mat4 uCameraViewMat;
uniform mat4 uCameraProjectionMat;
uniform mat4 uCameraViewInverse;
uniform mat4 uCameraProjectionInverse;

////////////////////////////////////////////////////////////
////////////////////// Cone Tracing ////////////////////////
////////////////////////////////////////////////////////////

// Thanks to Will Pearce's article
// http://roar11.com/2015/07/screen-space-glossy-reflections/

const float kMaxSpecularExponent = 16.0;
const float kSpecularBias = 1.0;

float SpecularPowerToConeAngle(float specularPower) {
    // based on phong distribution model
    if(specularPower >= exp2(kMaxSpecularExponent)) {
        return 0.0f;
    }
    const float xi = 0.244f;
    float exponent = 1.0f / (specularPower + 1.0f);
    return acos(pow(xi, exponent));
}

// https://seblagarde.wordpress.com/2012/06/10/amd-cubemapgen-for-physically-based-rendering/
float RoughnessToSpecularPower(float roughness) {
    float gloss = 1.0f - roughness;
    return exp2(kMaxSpecularExponent * gloss + kSpecularBias);
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

vec4 ConeSampleWeightedColor(vec2 samplePos, float mipChannel, float gloss) {
    vec3 sampleColor = textureLod(uReflections, samplePos, mipChannel).rgb;
    sampleColor *= kDenormalizationFactor;
    return vec4(sampleColor * gloss, gloss);
}

float IsoscelesTriangleNextAdjacent(float adjacentLength, float incircleRadius) {
    // subtract the diameter of the incircle to get the adjacent side of the next level on the cone
    return adjacentLength - (incircleRadius * 2.0f);
}

////////////////////////////////////////////////////////////
////////////////////////// Main ////////////////////////////
////////////////////////////////////////////////////////////

void main() {
    GBuffer gBuffer     = DecodeGBuffer(uGBufferAlbedoRoughnessMetalnessAONormal, vTexCoords);

    float roughness     = gBuffer.roughness;

    // Explicitly reading from 0 LOD because texture comes from a postprocess texture pool
    // and is a subject to mipmapping
    vec4 rayHitInfo = textureLod(uRayHitInfo, vTexCoords, 0);

    if (rayHitInfo.a == 0) {
        oOutputColor = vec4(0.0, 0.0, 0.0, 1.0);
        return;
    }

    float depth = textureLod(uGBufferHiZBuffer, vTexCoords, 0).r;

    vec3 raySS = rayHitInfo.xyz;
    vec3 positionSS = vec3(vTexCoords, depth);

    // Get specular power from roughness
    float gloss = 1.0f - roughness;
    float specularPower = RoughnessToSpecularPower(roughness);

    // Convert to cone angle (maximum extent of the specular lobe aperture)
    // Only want half the full cone angle since we're slicing the isosceles triangle in half to get a right triangle
    float coneTheta = SpecularPowerToConeAngle(specularPower) * 0.5f;

    // P1 = positionSS, P2 = raySS, adjacent length = ||P2 - P1||
    vec2 deltaP = raySS.xy - positionSS.xy;
    float adjacentLength = length(deltaP);
    vec2 adjacentUnit = normalize(deltaP);

    vec4 totalColor = vec4(0.0);
    float remainingAlpha = 1.0f;
    float maxMipLevel = float(uMipCount) - 1.0f;
    float glossMult = gloss;

    vec2 texSize = vec2(textureSize(uReflections, 0));

    // Cone-tracing using an isosceles triangle to approximate a cone in screen space
    for(int i = 0; i < 7; ++i) {
        // Intersection length is the adjacent side, get the opposite side using trig
        float oppositeLength = IsoscelesTriangleOpposite(adjacentLength, coneTheta);

        // Calculate in-radius of the isosceles triangle
        float incircleSize = IsoscelesTriangleInscribedCircleRadius(oppositeLength, adjacentLength);

        // Get the sample position in screen space
        vec2 samplePos = positionSS.xy + adjacentUnit * (adjacentLength - incircleSize);

        // Convert the in-radius into screen size then check what power N to raise 2 to reach it - that power N becomes mip level to sample from
        float mipChannel = clamp(log2(incircleSize * max(texSize.x, texSize.y)), 0.0, maxMipLevel);

        /*
         * Read color and accumulate it using trilinear filtering and weight it.
         * Uses pre-convolved image (color buffer) and glossiness to weigh color contributions.
         * Visibility is accumulated in the alpha channel. Break if visibility is 100% or greater (>= 1.0f).
         */
        vec4 newColor = ConeSampleWeightedColor(samplePos, mipChannel, glossMult);

        remainingAlpha -= newColor.a;

        if(remainingAlpha < 0.0f) {
            newColor.rgb *= (1.0f - abs(remainingAlpha));
        }

        totalColor += newColor;

        if(totalColor.a >= 1.0f) {
            break;
        }

        adjacentLength = IsoscelesTriangleNextAdjacent(adjacentLength, incircleSize);
        glossMult *= gloss;
    }

    oOutputColor = vec4(totalColor.rgb, 1.0);
}
