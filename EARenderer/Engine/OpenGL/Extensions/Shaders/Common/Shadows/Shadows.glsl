#include "Constants.glsl"
#include "Lights.glsl"
#include "Geometry.glsl"

/////////////////// HEPLERS ///////////////////////

// https://www.gamedev.net/articles/programming/graphics/contact-hardening-soft-shadows-made-fast-r4906/
//
vec2 VogelDiskSample(int sampleIndex, int samplesCount, float phi) {
    float GoldenAngle = 2.4f;

    float r = sqrt(float(sampleIndex) + 0.5f) / sqrt(float(samplesCount));
    float theta = float(sampleIndex) * GoldenAngle + phi;

    float sine = sin(theta);
    float cosine = cos(theta);

    return vec2(r * cosine, r * sine);
}

// Optimal kernel scaling for a given shadow map size and sample count
//
// * shadowMapSize - Dimensions of the shadow texture
// * samplingKernelSize - Length of sampling kernel's side
//
vec2 VogelDiskScale(vec2 shadowMapSize, int samplingKernelSize) {
    vec2 texelSize = 2.0 / shadowMapSize;
    return texelSize * samplingKernelSize;
}

float InterleavedGradientNoise(vec2 screenCoordinate) {
    vec3 magic = vec3(0.06711056f, 0.00583715f, 52.9829189f);
    return fract(magic.z * fract(dot(screenCoordinate, magic.xy)));
}

float AvgBlockersDepthToPenumbra(float z_shadowMapView, float avgBlockersDepth, float lightArea) {
    return lightArea * (z_shadowMapView - avgBlockersDepth) / avgBlockersDepth;
}
