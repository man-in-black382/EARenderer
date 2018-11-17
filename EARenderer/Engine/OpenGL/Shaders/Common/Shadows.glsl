#include "Constants.glsl"

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

float InterleavedGradientNoise(vec2 screenCoordinate) {
    vec3 magic = vec3(0.06711056f, 0.00583715f, 52.9829189f);
    return fract(magic.z * fract(dot(screenCoordinate, magic.xy)));
}

float AvgBlockersDepthToPenumbra(float z_shadowMapView, float avgBlockersDepth) {
    float penumbra = (z_shadowMapView - avgBlockersDepth) / avgBlockersDepth;
    penumbra *= penumbra;
    return clamp(80.0f * penumbra, 0.0, 1.0);
}

float Penumbra(float gradientNoise, vec2 shadowMapUV, float z_shadowMapView, int samplesCount) {
    float avgBlockersDepth = 0.0f;
    float blockersCount = 0.0f;
    
//    for(int i = 0; i < samplesCount; i ++) {
//        vec2 sampleUV = VogelDiskSample(i, samplesCount, gradientNoise);
//        sampleUV = shadowMapUV + penumbraFilterMaxSize * sampleUV;
//        
//        float sampleDepth = shadowMapTexture.SampleLevel(pointClampSampler, sampleUV, 0).x;
//        
//        if(sampleDepth < z_shadowMapView) {
//            avgBlockersDepth += sampleDepth;
//            blockersCount += 1.0f;
//        }
//    }
//    
//    if(blockersCount > 0.0f) {
//        avgBlockersDepth /= blockersCount;
//        return AvgBlockersDepthToPenumbra(z_shadowMapView, avgBlockersDepth);
//    }

    return 0.0f;
}

int ShadowCascadeIndex(vec3 surfaceWorldPosition, // World position of the surface point
                       mat4 CSMSplitSpaceMat,     // Space in which splits were calculated
                       int depthSplitsAxis,       // Axis containing the splits. 0 - x, 1 - y, 2 - z
                       float depthSplits[MaximumShadowCascadesCount]) // Array of split positions that separate cascades from one another
{
    vec4 posInCSMSplitSpace = CSMSplitSpaceMat * vec4(surfaceWorldPosition, 1.0);
    
    vec3 projCoords = posInCSMSplitSpace.xyz / posInCSMSplitSpace.w;
    // No need to transform to [0,1] range,
    // because splits passed from client are in [-1; 1]
    
    float locationOnSplitAxis = projCoords[depthSplitsAxis];
    
    for (int i = 0; i < MaximumShadowCascadesCount; ++i) {
        if (locationOnSplitAxis < depthSplits[i]) {
            return i;
        }
    }
}

float DirectionalShadow(vec3 surfaceWorldPosition, // World position of the surface point
                        vec3 surfaceNormal,        // World normal of the surface point
                        vec3 surfaceToLight,       // World Surface to light vector
                        int cascadeIndex,          // Index of the shadow cascade containing the surface point
                        mat4 lightSpaceMatrices[MaximumShadowCascadesCount], // Transformation matrices for each cascade
                        sampler2DArrayShadow shadowMapArray) // Shadow map array with shadow maps for each cascade
{
    mat4 relevantLightMatrix = lightSpaceMatrices[cascadeIndex];
    vec4 positionInLightSpace = relevantLightMatrix * vec4(surfaceWorldPosition, 1.0);
    
    vec3 projectedCoords = positionInLightSpace.xyz / positionInLightSpace.w;
    projectedCoords = projectedCoords * 0.5 + 0.5; // To [0; 1]
    
    vec2 shadowMapUV = projectedCoords.xy;
    
    // Get depth of current fragment from light's perspective
    float currentDepth = projectedCoords.z;

    float sine = 1.0 - dot(surfaceNormal, surfaceToLight);
    float bias = max(0.005 * sine, 0.005);
    
    float biasedDepth = currentDepth - bias;
    
    float gradientNoise = InterleavedGradientNoise(gl_FragCoord.xy);
    float shadow = 0.0f;
    float shadowFilterMaxSize = 0.002;
    
    for(int i = 0; i < 16; i++) {
        vec2 sampleUV = VogelDiskSample(i, 16, gradientNoise);
        sampleUV = shadowMapUV + sampleUV /* penumbra*/ * shadowFilterMaxSize;
        shadow += texture(shadowMapArray, vec4(sampleUV, cascadeIndex, biasedDepth));
    }
    shadow /= 16.0f;
    
    // Leverage hardware PCF
    return shadow;
}
