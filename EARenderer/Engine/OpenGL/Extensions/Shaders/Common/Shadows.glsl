#include "Constants.glsl"
#include "Lights.glsl"
#include "Transforms.glsl"

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
    vec2 texelSize = 1.0 / shadowMapSize;
    return texelSize * samplingKernelSize;
}

float InterleavedGradientNoise(vec2 screenCoordinate) {
    vec3 magic = vec3(0.06711056f, 0.00583715f, 52.9829189f);
    return fract(magic.z * fract(dot(screenCoordinate, magic.xy)));
}

float AvgBlockersDepthToPenumbra(float z_shadowMapView, float avgBlockersDepth, float lightArea) {
    float penumbra = (z_shadowMapView - avgBlockersDepth) / avgBlockersDepth;
    return clamp(lightArea * penumbra, 0.0, 1.0);
//    return clamp(10.0 * penumbra, 0.0, 1.0);
}

// Transforms light-to-surface vector to hyperbolic normalized depth from light's perspective
float DepthFromPointLightPerspective(PointLight light, vec3 surfaceWorldPosition) {
    vec3 absSurfaceToLight = abs(light.position - surfaceWorldPosition);
    // Largest component will always be Z component 
    float localZ = max(absSurfaceToLight.x, max(absSurfaceToLight.y, absSurfaceToLight.z));

    float f = light.farPlane;
    float n = light.nearPlane;
    
    // Transforming to NDC depth
    float depth = (f + n) / (f - n) - (2.0 * f * n) / (f - n) / localZ;
    
    // Normalizing
    return (depth + 1.0) * 0.5;
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

    return 0;
}

/////////////////// PENUMBRA ///////////////////////

float DirectionalPenumbra(vec3 surfaceWorldPosition, // World position of the surface point
                          int cascadeIndex,          // Index of the shadow cascade containing the surface point
                          mat4 lightSpaceMatrices[MaximumShadowCascadesCount],
                          DirectionalLight light,
                          sampler2DArray bilinearSampler)
{
    // Constants that should be refactored into configurable parameters
    const int KernelSize = 2;
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

float OmnidirectionalPenumbra(vec3 surfaceWorldPosition, // World position of the surface point
                              PointLight light,
                              samplerCube bilinearSampler) // Shadow map bilinear sampler
{
    // Constants that should be refactored into configurable parameters
    const int KernelSize = 2;
    const int VogelDiskSampleCount = KernelSize * KernelSize; // kernelSize * kernelSize
    //

    vec3 surfaceToLight = light.position - surfaceWorldPosition;
    vec3 cubeSamplingVector = normalize(-surfaceToLight);
    mat4 rotationMatrix = RotationMatrix(cubeSamplingVector);

    // Get depth of current fragment from light's perspective
    float surfaceDepth = DepthFromPointLightPerspective(light, surfaceWorldPosition);

    float gradientNoise = InterleavedGradientNoise(gl_FragCoord.xy);
    vec2 shadowMapSize = textureSize(bilinearSampler, 0).xy;
    vec2 penumbraFilterMaxSize = VogelDiskScale(shadowMapSize, KernelSize);

    float avgBlockersDepth = 0.0f;
    float blockersCount = 0.0f;

    for(int i = 0; i < VogelDiskSampleCount; i++) {
        vec2 vogelDiskSample = VogelDiskSample(i, VogelDiskSampleCount, gradientNoise);
        vec3 sampleVector = (rotationMatrix * vec4(vogelDiskSample, 1.0, 0.0)).xyz;

        float sampleDepth = texture(bilinearSampler, sampleVector).r;

        if(sampleDepth < surfaceDepth) {
            avgBlockersDepth += sampleDepth;
            blockersCount += 1.0f;
        }
    }

    if(blockersCount > 0.0f) {
        avgBlockersDepth /= blockersCount;
        return AvgBlockersDepthToPenumbra(surfaceDepth, avgBlockersDepth, light.area);
    }

    return 0.0;
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
    const float KernelScale = 4.0; // Can be used to increase sampling window when used in conjunction with penumbra
    const vec2 BiasLimits = vec2(0.002, 0.004);
    //
    
    mat4 relevantLightMatrix = lightSpaceMatrices[cascadeIndex];
    vec4 positionInLightSpace = relevantLightMatrix * vec4(surfaceWorldPosition, 1.0);
    
    vec3 projectedCoords = positionInLightSpace.xyz / positionInLightSpace.w;
    projectedCoords = projectedCoords * 0.5 + 0.5; // To [0; 1]
    
    vec2 shadowMapUV = projectedCoords.xy;
    
    // Get depth of current fragment from light's perspective
    float currentDepth = projectedCoords.z;

    float sine = 1.0 - dot(surfaceNormal, surfaceToLight);
    float bias = max(BiasLimits.x, BiasLimits.y * sine);
    float biasedDepth = currentDepth - bias;

    #ifndef SHADOW_NO_PCF
    float gradientNoise = InterleavedGradientNoise(gl_FragCoord.xy);
    vec2 shadowMapSize = textureSize(comparisonSampler, 0).xy;
    vec2 shadowFilterMaxSize = KernelScale * VogelDiskScale(shadowMapSize, KernelSize);
    
    float shadow = 0.0f;

    for(int i = 0; i < VogelDiskSampleCount; i++) {
        vec2 sampleUV = VogelDiskSample(i, VogelDiskSampleCount, gradientNoise);
        sampleUV = shadowMapUV + sampleUV * penumbra * shadowFilterMaxSize;
        shadow += texture(comparisonSampler, vec4(sampleUV, cascadeIndex, biasedDepth));
    }
    
    shadow /= float(VogelDiskSampleCount);
    return shadow;
    #else
    return texture(comparisonSampler, vec4(shadowMapUV, cascadeIndex, biasedDepth));
    #endif
}

float OmnidirectionalShadow(vec3 surfaceWorldPosition, // World position of the surface point
                            vec3 surfaceNormal,        // World normal of the surface point
                            PointLight light,
                            samplerCubeShadow comparisonSampler, // Shadow cube map for point light. Comparison sampler for hardware PCF.
                            float penumbra) // Penumbra value for the given surface point
{
    // Constants that should be refactored into configurable parameters
    const int KernelSize = 4;
    const int VogelDiskSampleCount = KernelSize * KernelSize; // kernelSize * kernelSize
    const float KernelScale = 4.0; // Can be used to increase sampling window when used in conjunction with penumbra
    const vec2 BiasLimits = vec2(0.0002, 0.0005);
    //

    vec3 surfaceToLight = light.position - surfaceWorldPosition;
    vec3 cubeSamplingVector = normalize(-surfaceToLight);
    // Matrix to transform 2D vogel disk samples to 3D sampling vectors
    mat4 rotationMatrix = RotationMatrix(cubeSamplingVector);

    // Get depth of current fragment from light's perspective
    float currentDepth = DepthFromPointLightPerspective(light, surfaceWorldPosition);
    float sine = 1.0 - dot(surfaceNormal, normalize(surfaceToLight));
    float bias = max(BiasLimits.x, BiasLimits.y * sine);
    float biasedDepth = currentDepth - bias;

    #ifndef SHADOW_NO_PCF
    float gradientNoise = InterleavedGradientNoise(gl_FragCoord.xy);
    vec2 shadowMapSize = textureSize(comparisonSampler, 0).xy;
    vec2 shadowFilterMaxSize = KernelScale * VogelDiskScale(shadowMapSize, KernelSize);

    float shadow = 0.0f;

    for(int i = 0; i < VogelDiskSampleCount; i++) {
        vec2 vogelDiskSample = VogelDiskSample(i, VogelDiskSampleCount, gradientNoise) * penumbra * shadowFilterMaxSize;
        vec3 sampleVector = (rotationMatrix * vec4(vogelDiskSample, 1.0, 0.0)).xyz;
        shadow += texture(comparisonSampler, vec4(sampleVector, biasedDepth));
    }

    shadow /= float(VogelDiskSampleCount);
    return shadow;
    #else
    return texture(comparisonSampler, vec4(cubeSamplingVector, biasedDepth));
    #endif
}
