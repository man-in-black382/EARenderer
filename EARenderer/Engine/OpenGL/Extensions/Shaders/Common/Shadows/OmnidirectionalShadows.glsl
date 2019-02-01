#include "Shadows.glsl"
#include "CubeMapUtils.glsl"

// Transforms light-to-surface vector to hyperbolic normalized depth from light's perspective
float DepthFromPointLightPerspective(PointLight light, vec3 surfaceWorldPosition) {
    float f = light.farPlane;
    float n = light.nearPlane;

    vec3 absLightToSurface = abs(surfaceWorldPosition - light.position.xyz);
    // Largest component will always be Z component
    float localZ = max(absLightToSurface.x, max(absLightToSurface.y, absLightToSurface.z));
    // Transforming to NDC depth
    float depth = (f + n) / (f - n) - (2.0 * f * n) / (f - n) / localZ;
    // Normalizing
    return (depth + 1.0) * 0.5;
}

// Adaptive Depth Bias for Soft Shadows
// https://dspace5.zcu.cz/bitstream/11025/29520/1/Ehm.pdf

float OmnidirectionalAdaptiveEpsilon(
    PointLight light,
    vec3 lightDirection, // Surface-to-light vector. Expected to be normalized.
    vec3 surfaceNormal, // Surface normal in world space.
    float surfaceDepth, // Surface hyperbolic depth in light space.
    float sceneBBDiagonal) // Length of scene's bounding box diagonal.
{
    float num = light.farPlane - surfaceDepth * (light.farPlane - light.nearPlane);
    num *= num;

    float denom = light.farPlane * light.nearPlane * (light.farPlane - light.nearPlane);
    float dotProduct = dot(surfaceNormal, lightDirection);
    float scaleFactor = min(1.0 / (dotProduct * dotProduct), 100.0);

    return (num / denom) * sceneBBDiagonal * light.shadowBias * scaleFactor;
}

float OmnidirectionalPotentialOccluderDepth(
    PointLight light,
    vec3 surfaceWorldPosition,
    vec3 surfaceWorldNormal,
    float surfaceDepth, // Hyperbolic depth [0; 1], in light space
    vec3 texCoords, // Cube texture coords. S, T and Face index
    vec2 shadowMapSize)
{
    // 1) Find center of the shadow map texel in NDC space that corresponds to the current fragment.
    vec2 texelIndex = texCoords.st * (shadowMapSize - 1); // Go from [0; 1] to [0; texSize - 1] (from [0.1, 0.55] to [102.3, 563.65])
    texelIndex = floor(texelIndex); // Go for the actual index (from [102.3, 563.65] to [102.0, 563.0])
    texelIndex += 0.5; // Go to the center (from [102.0, 563.0] to [102.5, 563.5])

    vec2 texelCenter = texelIndex / (shadowMapSize - 1); // Obtain normalized texture coordinates of the texel center
    vec2 ndcTexelCenter = texelCenter * 2.0 - 1.0; // From [0; 1] to [-1; 1]
    vec4 nearPlaneNDCCenter = vec4(ndcTexelCenter, -1.0, 1.0); // One can say that texture data lies on the near plane which is -1 in NDC space

    // 2) Find out which view-projection matrix was used to transform current fragment into light's space
    int cubeFace = int(texCoords.z);
    mat4 view = ViewMatrixForCubeFace(light.position.xyz, cubeFace);

    vec3 surfaceLightSpacePosition = (view * vec4(surfaceWorldPosition, 1.0)).xyz;
    vec3 surfaceLightSpaceNormal = (view * vec4(surfaceWorldNormal, 0.0)).xyz;

    // 3) Get light-to-texel center vector in world space to be able to perform ray tracing against surface defined by current fragment's position and normal
    vec4 lightToTexelCenter = light.inverseProjection * nearPlaneNDCCenter; // Vector from light to texel center in world space
    lightToTexelCenter.xyz /= lightToTexelCenter.w;
    lightToTexelCenter.xyz = normalize(lightToTexelCenter.xyz);

    // 4) Get an intersection point
    float t = 0.0;
    bool intersected = RayPlaneIntersection(surfaceLightSpacePosition, surfaceLightSpaceNormal, vec3(0.0), lightToTexelCenter.xyz, t);

    // 5) Now transform intersection point back to NDC and normalized texture/depth coordinates.
    // Difference between depths of the potential occluder and the current fragment is our optimal bias value.
    vec3 potentialOccluder = lightToTexelCenter.xyz * t;
    vec4 potentialOccluderNDC = light.projection * vec4(potentialOccluder, 1.0);
    potentialOccluderNDC /= potentialOccluderNDC.w;
    potentialOccluderNDC = potentialOccluderNDC * 0.5 + 0.5;

    return potentialOccluderNDC.z;
}

float OmnidirectionalPenumbra(vec3 surfaceWorldPosition, // World position of the surface point
                              vec3 surfaceNormal,
                              PointLight light,
                              samplerCube bilinearSampler) // Shadow map bilinear sampler
{
    // Constants that should be refactored into configurable parameters
    float KernelSize = 4.0;
    int VogelDiskSampleCount = int(KernelSize * KernelSize);

    vec3 surfaceToLight = light.position.xyz - surfaceWorldPosition;
    vec3 lightDirection = normalize(-surfaceToLight);
    mat4 rotationMatrix = RotationMatrix(lightDirection);

    // Get depth of current fragment from light's perspective
    float surfaceDepth = DepthFromPointLightPerspective(light, surfaceWorldPosition);

    float gradientNoise = InterleavedGradientNoise(gl_FragCoord.xy);
    vec2 shadowMapSize = textureSize(bilinearSampler, 0).xy;
    vec2 penumbraFilterMaxSize = VogelDiskScale(shadowMapSize, int(KernelSize));

    float avgBlockersDepth = 0.0f;
    float blockersCount = 0.0f;

    for(int i = 0; i < VogelDiskSampleCount; i++) {
        vec2 vogelDiskSample = VogelDiskSample(i, VogelDiskSampleCount, gradientNoise);
        vec3 sampleVector = (rotationMatrix * vec4(vogelDiskSample, 1.0, 0.0)).xyz;

        vec3 texCoords = CubeMapTextureCoords(sampleVector);
        float pod = OmnidirectionalPotentialOccluderDepth(light, surfaceWorldPosition, surfaceNormal, surfaceDepth, texCoords, shadowMapSize);

        vec3 surfaceToLight = -normalize(sampleVector);
        float epsilon = OmnidirectionalAdaptiveEpsilon(light, surfaceToLight, surfaceNormal, pod, light.shadowBias);
        float bias = max(0.0, surfaceDepth - pod);
        float biasedDepth = surfaceDepth - bias - epsilon;

        float sampleDepth = texture(bilinearSampler, sampleVector).r;

        if(sampleDepth < biasedDepth) {
            avgBlockersDepth += sampleDepth;
            blockersCount += 1.0f;
        }
    }

    if(blockersCount > 0.0f) {
        avgBlockersDepth /= blockersCount;
        return AvgBlockersDepthToPenumbra(surfaceDepth, avgBlockersDepth, light.area);
    }

    return 1.0;
}

float OmnidirectionalShadow(vec3 surfaceWorldPosition, // World position of the surface point
                            vec3 surfaceNormal,        // World normal of the surface point
                            PointLight light,
                            samplerCubeShadow comparisonSampler, // Shadow cube map for point light. Comparison sampler for hardware PCF.
                            float penumbra) // Penumbra value for the given surface point
{
    vec3 lightToSurface = surfaceWorldPosition - light.position.xyz;

    // Matrix to transform 2D vogel disk samples to 3D sampling vectors
    mat4 rotationMatrix = RotationMatrix(lightToSurface);

    // Get depth of current fragment from light's perspective
    float currentDepth = DepthFromPointLightPerspective(light, surfaceWorldPosition);
    vec2 shadowMapSize = textureSize(comparisonSampler, 0).xy;

    #ifndef SHADOW_NO_PCF
    vec2 texelSize = 1.0 / shadowMapSize;
    vec2 twoTexelSize = 2.0 * texelSize;
    vec2 kernelSize = twoTexelSize * 4.0;

    float gradientNoise = InterleavedGradientNoise(gl_FragCoord.xy);
    int VogelDiskSampleCount = 16;

    // Adaptive bias
    // A fast way of retrieving neighbour texel for cubemap needs to be invented. An approach proposed in the paper
    // is intended to work with orthographic projection and a single shadow map for directional light source.
    // Resorting to a more expensive O(N) instead of O(1) approach until then.
    //
//    vec3 texCoords = CubeMapTextureCoords(lightToSurface);
//    float pod = OmnidirectionalPotentialOccluderDepth(light, surfaceWorldPosition, surfaceNormal, currentDepth, texCoords, shadowMapSize);
//    float podX = OmnidirectionalPotentialOccluderDepth(light, surfaceWorldPosition, surfaceNormal, currentDepth, texCoords + (texelSize.x, 0.0, 0.0), shadowMapSize);
//    float podY = OmnidirectionalPotentialOccluderDepth(light, surfaceWorldPosition, surfaceNormal, currentDepth, texCoords + (0.0, texelSize.y, 0.0), shadowMapSize);
//
//    float deltaBiasX = podX - pod;
//    float deltaBiasY = podY - pod;
    //

    float shadow = 0.0f;

    for(int i = 0; i < VogelDiskSampleCount; i++) {
        vec2 vogelDiskSample = VogelDiskSample(i, VogelDiskSampleCount, gradientNoise) * kernelSize * penumbra;
        vec3 sampleVector = (rotationMatrix * vec4(vogelDiskSample, 1.0, 0.0)).xyz;

        // Adaptive bias. More robust approach. Commented until fast neighbour texel search is implemented.
        //
//        float currentPOD = pod + vogelDiskSample.x * deltaBiasX + vogelDiskSample.y * deltaBiasY;
//        vec3 surfaceToLight = -normalize(sampleVector);
//        float epsilon = OmnidirectionalAdaptiveEpsilon(light, surfaceToLight, surfaceNormal, currentPOD, 1.0);
//        float bias = max(0.0, currentDepth - currentPOD);
//        float biasedDepth = currentDepth - bias - epsilon;
        //

        vec3 texCoords = CubeMapTextureCoords(sampleVector);
        float pod = OmnidirectionalPotentialOccluderDepth(light, surfaceWorldPosition, surfaceNormal, currentDepth, texCoords, shadowMapSize);

        vec3 surfaceToLight = -normalize(sampleVector);
        float epsilon = OmnidirectionalAdaptiveEpsilon(light, surfaceToLight, surfaceNormal, pod, 1.0);

        float bias = max(0.0, currentDepth - pod);
        float biasedDepth = currentDepth - bias - epsilon;

        shadow += texture(comparisonSampler, vec4(sampleVector, biasedDepth));
    }

    shadow /= float(VogelDiskSampleCount);
    return shadow;
    #else
    return texture(comparisonSampler, vec4(lightToSurface, biasedDepth));
    #endif
}
