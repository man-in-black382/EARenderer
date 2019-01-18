#include "Shadows.glsl"

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

    return 0;
}

float DirectionalAdaptiveEpsilon(vec3 surfaceNormal, vec3 surfaceToLight, float K) {
    float dotProduct = dot(surfaceNormal, surfaceToLight);
    float scaleFactor = min(1.0 / (dotProduct * dotProduct), 100.0);
    return K * scaleFactor;
}

///////////////// PENUMBRA ///////////////////////

float DirectionalPenumbra(vec3 surfaceWorldPosition, // World position of the surface point
                          int cascadeIndex,          // Index of the shadow cascade containing the surface point
                          mat4 lightSpaceMatrices[MaximumShadowCascadesCount],
                          DirectionalLight light,
                          sampler2DArray bilinearSampler)
{
    // Constants that should be refactored into configurable parameters
    const int KernelSize = 4;
    const int VogelDiskSampleCount = KernelSize * KernelSize; // kernelSize * kernelSize
    //

    mat4 relevantLightMatrix = lightSpaceMatrices[cascadeIndex];
    vec4 positionInLightSpace = relevantLightMatrix * vec4(surfaceWorldPosition, 1.0);

    vec3 projectedCoords = positionInLightSpace.xyz / positionInLightSpace.w;
    projectedCoords = projectedCoords * 0.5 + 0.5; // To [0; 1]

    vec2 shadowMapUV = projectedCoords.xy;
    float surfaceDepth = projectedCoords.z;
    
    float avgBlockersDepth = 0.0f;
    float blockersCount = 0.0f;
    float gradientNoise = InterleavedGradientNoise(gl_FragCoord.xy);

    vec2 shadowMapSize = textureSize(bilinearSampler, 0).xy;
    vec2 penumbraFilterMaxSize = VogelDiskScale(shadowMapSize, KernelSize);

    for(int i = 0; i < VogelDiskSampleCount; i++) {
        vec2 sampleUV = VogelDiskSample(i, VogelDiskSampleCount, gradientNoise);
        sampleUV = shadowMapUV + penumbraFilterMaxSize * sampleUV;

        float sampleDepth = texture(bilinearSampler, vec3(sampleUV, cascadeIndex)).r;

        if(sampleDepth < surfaceDepth) {
            avgBlockersDepth += sampleDepth;
            blockersCount += 1.0f;
        }
    }

    if(blockersCount > 0.0f) {
        avgBlockersDepth /= blockersCount;
        return AvgBlockersDepthToPenumbra(surfaceDepth, avgBlockersDepth, light.area);
    }

    return 1.0f;
}

/////////////////// SHADOWS ///////////////////////

float DirectionalShadow(vec3 surfaceWorldPosition, // World position of the surface point
                        vec3 surfaceNormal,        // World normal of the surface point
                        vec3 surfaceToLight,       // World Surface to light vector. Expected to be normalized.
                        int cascadeIndex,          // Index of the shadow cascade containing the surface point
                        mat4 lightSpaceMatrices[MaximumShadowCascadesCount], // Transformation matrices for each cascade
                        sampler2DArrayShadow comparisonSampler, // Shadow map array with shadow maps for each cascade. Comparison sampler for hardware PCF.
                        float penumbra) // Penumbra value for the given surface point
{
    // Constants that should be refactored into configurable parameters
    const int KernelSize = 4;
    const int VogelDiskSampleCount = KernelSize * KernelSize; // kernelSize * kernelSize
    //
    
    mat4 relevantLightMatrix = lightSpaceMatrices[cascadeIndex];
    vec4 positionInLightSpace = relevantLightMatrix * vec4(surfaceWorldPosition, 1.0);
    
    vec3 projectedCoords = positionInLightSpace.xyz / positionInLightSpace.w;
    projectedCoords = projectedCoords * 0.5 + 0.5; // To [0; 1]
    
    vec2 shadowMapUV = projectedCoords.xy;
    
    // Get depth of current fragment from light's perspective
    float currentDepth = projectedCoords.z;
    float biasedDepth = currentDepth - DirectionalAdaptiveEpsilon(surfaceNormal, surfaceToLight, 0.00001);

    #ifndef SHADOW_NO_PCF
    float gradientNoise = InterleavedGradientNoise(gl_FragCoord.xy);
    vec2 shadowMapSize = textureSize(comparisonSampler, 0).xy;
    vec2 shadowFilterMaxSize = VogelDiskScale(shadowMapSize, KernelSize);
    
    float shadow = 0.0f;

    for(int i = 0; i < VogelDiskSampleCount; i++) {
        vec2 sampleUV = VogelDiskSample(i, VogelDiskSampleCount, gradientNoise);
        sampleUV = shadowMapUV + sampleUV * shadowFilterMaxSize * penumbra;
        shadow += texture(comparisonSampler, vec4(sampleUV, cascadeIndex, biasedDepth));
    }
    
    shadow /= float(VogelDiskSampleCount);
    return shadow;
    #else
    return texture(comparisonSampler, vec4(shadowMapUV, cascadeIndex, biasedDepth));
    #endif
}