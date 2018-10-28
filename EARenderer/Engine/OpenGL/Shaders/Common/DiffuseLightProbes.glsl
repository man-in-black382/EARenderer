#include "Types.glsl"
#include "SphericalHarmonics.glsl"

// Packing scheme:
//                         Y    Y      Y    Y       Y    Y       Y     Y      Y   Co
// 9 Luma coefficients  [(L00, L11), (L10, L1_1), (L21, L2_1), (L2_2, L20), (L22, L00),
// 9 Co and 9 Cg coeff.   Cg   Co     Co   Co      Cg   Cg      Cg   Co      Co    Co
// are mixed up          (L00, L11), (L10, L1_1), (L11, L10), (L1_1, L21), (L2_1, L2_2),
//                        Co   Co     Cg   Cg      Cg    Cg     Cg
//                       (L20, L22), (L21, L2_1), (L2_2, L20), (L22, 0.0)]
//
SH UnpackSH_333(usampler3D gridSHMap0, // Each uint sampled from these textures
                usampler3D gridSHMap1, // contains 2 encoded half-precision float values
                usampler3D gridSHMap2, //
                usampler3D gridSHMap3,
                ivec3 texCoords) // Integer coordinates [0; gridSize - 1]
{
    SH sh = ZeroSH();

    uvec4 shMap0Data = texelFetch(gridSHMap0, texCoords, 0);
    uvec4 shMap1Data = texelFetch(gridSHMap1, texCoords, 0);
    uvec4 shMap2Data = texelFetch(gridSHMap2, texCoords, 0);
    uvec4 shMap3Data = texelFetch(gridSHMap3, texCoords, 0);

    float range = uintBitsToFloat(shMap0Data.r);

    vec2 pair0 = UnpackSnorm2x16(shMap0Data.g, range);  vec2 pair1 = UnpackSnorm2x16(shMap0Data.b, range);
    vec2 pair2 = UnpackSnorm2x16(shMap0Data.a, range);  vec2 pair3 = UnpackSnorm2x16(shMap1Data.r, range);
    vec2 pair4 = UnpackSnorm2x16(shMap1Data.g, range);  vec2 pair5 = UnpackSnorm2x16(shMap1Data.b, range);
    vec2 pair6 = UnpackSnorm2x16(shMap1Data.a, range);  vec2 pair7 = UnpackSnorm2x16(shMap2Data.r, range);
    vec2 pair8 = UnpackSnorm2x16(shMap2Data.g, range);  vec2 pair9 = UnpackSnorm2x16(shMap2Data.b, range);
    vec2 pair10 = UnpackSnorm2x16(shMap2Data.a, range); vec2 pair11 = UnpackSnorm2x16(shMap3Data.r, range);
    vec2 pair12 = UnpackSnorm2x16(shMap3Data.g, range); vec2 pair13 = UnpackSnorm2x16(shMap3Data.b, range);

    sh.L00.r = pair0.x;   sh.L11.r = pair0.y;   sh.L10.r = pair1.x;   sh.L1_1.r = pair1.y;
    sh.L21.r = pair2.x;   sh.L2_1.r = pair2.y;  sh.L2_2.r = pair3.x;  sh.L20.r = pair3.y;
    sh.L22.r = pair4.x;   sh.L00.g = pair4.y;   sh.L00.b = pair5.x;   sh.L11.g = pair5.y;
    sh.L10.g = pair6.x,   sh.L1_1.g = pair6.y;  sh.L11.b = pair7.x;   sh.L10.b = pair7.y;
    sh.L1_1.b = pair8.x;  sh.L21.g = pair8.y;   sh.L2_1.g = pair9.x;  sh.L2_2.g = pair9.y;
    sh.L20.g = pair10.x,  sh.L22.g = pair10.y;  sh.L21.b = pair11.x,  sh.L2_1.b = pair11.y;
    sh.L2_2.b = pair12.x; sh.L20.b = pair12.y;  sh.L22.b = pair13.x;

    return sh;
}

SH UnpackSH_322(usampler3D gridSHMap0,
                usampler3D gridSHMap1,
                usampler3D gridSHMap2,
                ivec3 texCoords) // Integer coordinates [0; gridSize - 1]
{
    SH sh = ZeroSH();

    uvec4 shMap0Data = texelFetch(gridSHMap0, texCoords, 0);
    uvec4 shMap1Data = texelFetch(gridSHMap1, texCoords, 0);
    uvec4 shMap2Data = texelFetch(gridSHMap2, texCoords, 0);

    float range = uintBitsToFloat(shMap0Data.r);

    vec2 pair0 = UnpackSnorm2x16(shMap0Data.g, range);  vec2 pair1 = UnpackSnorm2x16(shMap0Data.b, range);
    vec2 pair2 = UnpackSnorm2x16(shMap0Data.a, range);  vec2 pair3 = UnpackSnorm2x16(shMap1Data.r, range);
    vec2 pair4 = UnpackSnorm2x16(shMap1Data.g, range);  vec2 pair5 = UnpackSnorm2x16(shMap1Data.b, range);
    vec2 pair6 = UnpackSnorm2x16(shMap1Data.a, range);  vec2 pair7 = UnpackSnorm2x16(shMap2Data.r, range);
    vec2 pair8 = UnpackSnorm2x16(shMap2Data.g, range);

    sh.L00.r = pair0.x;   sh.L11.r = pair0.y;   sh.L10.r = pair1.x;   sh.L1_1.r = pair1.y;
    sh.L21.r = pair2.x;   sh.L2_1.r = pair2.y;  sh.L2_2.r = pair3.x;  sh.L20.r = pair3.y;
    sh.L22.r = pair4.x;   sh.L00.g = pair4.y;   sh.L00.b = pair5.x;   sh.L11.g = pair5.y;
    sh.L10.g = pair6.x,   sh.L1_1.g = pair6.y;  sh.L11.b = pair7.x;   sh.L10.b = pair7.y;
    sh.L1_1.b = pair8.x;

    return sh;
}

SH UnpackSH_311(usampler3D gridSHMap0,
                usampler3D gridSHMap1,
                ivec3 texCoords) // Integer coordinates [0; gridSize - 1]
{
    SH sh = ZeroSH();

    uvec4 shMap0Data = texelFetch(gridSHMap0, texCoords, 0);
    uvec4 shMap1Data = texelFetch(gridSHMap1, texCoords, 0);

    float range = uintBitsToFloat(shMap0Data.r);

    vec2 pair0 = UnpackSnorm2x16(shMap0Data.g, range);  vec2 pair1 = UnpackSnorm2x16(shMap0Data.b, range);
    vec2 pair2 = UnpackSnorm2x16(shMap0Data.a, range);  vec2 pair3 = UnpackSnorm2x16(shMap1Data.r, range);
    vec2 pair4 = UnpackSnorm2x16(shMap1Data.g, range);  vec2 pair5 = UnpackSnorm2x16(shMap1Data.b, range);

    sh.L00.r = pair0.x;   sh.L11.r = pair0.y;   sh.L10.r = pair1.x;   sh.L1_1.r = pair1.y;
    sh.L21.r = pair2.x;   sh.L2_1.r = pair2.y;  sh.L2_2.r = pair3.x;  sh.L20.r = pair3.y;
    sh.L22.r = pair4.x;   sh.L00.g = pair4.y;   sh.L00.b = pair5.x;

    return sh;
}

SH UnpackSH(usampler3D gridSHMap0,
            usampler3D gridSHMap1,
            usampler3D gridSHMap2,
            usampler3D gridSHMap3,
            ivec3 texCoords) // Integer coordinates [0; gridSize - 1]
{
#if defined (PROBE_SH_COMPRESSION_322)
    return UnpackSH_322(gridSHMap0, gridSHMap1, gridSHMap2, texCoords);
#elif defined(PROBE_SH_COMPRESSION_311)
    return UnpackSH_311(gridSHMap0, gridSHMap1, texCoords);
#else
    return UnpackSH_333(gridSHMap0, gridSHMap1, gridSHMap2, gridSHMap3, texCoords);
#endif
}

float ProbeOcclusionFactor(samplerBuffer probePositions, // A sequential buffer containing world positions for all probes in the scene
                           ivec3 probeGridPosition, // 3D integer position of a probe in probe grid ( [0; 5; 3] for example )
                           ivec3 gridSize, // Size of the probe grid
                           vec3 surfaceNormal, // Normal to some surface for which we want to determine an occlusion factor
                           vec3 worldPosition) // World position of such surface
{
    int gridWidth = gridSize.x;
    int gridHeight = gridSize.y;

    // [x + WIDTH * (y + HEIGHT * z)]
    int probeCoord1D = probeGridPosition.x + gridWidth * (probeGridPosition.y + gridHeight * probeGridPosition.z);

    vec3 probePosition = texelFetch(probePositions, probeCoord1D).xyz;

    vec3 surfaceToProbe = normalize(probePosition - worldPosition);
    float weight = max(0.0, dot(surfaceToProbe, surfaceNormal));
    return weight;
}

// Computes 8 interpolation weights given 2 corner points and a point of interest
vec8 TriLerp(vec3 pMin, vec3 pMax, vec3 p) {
    //
    //        5-------6
    //       /|      /|
    // Y    / |     / |
    // ^   1--|----2  |
    // |   |  4----|--7
    // |   | /     | /
    // |   0-------3
    // |
    //  -----------> X
    // 0 - min
    // 6 - max
    // Interpolation weights are in order: 0, 1, 2, 3, 4, 5, 6, 7

    vec3 extents = pMax - pMin;
    float divisorInv = 1.0 / (extents.x * extents.y * extents.z);

    vec8 weights;
    weights.value0 = (pMax.x - p.x) * (pMax.y - p.y) * (pMax.z - p.z) * divisorInv;
    weights.value1 = (pMax.x - p.x) * (p.y - pMin.y) * (pMax.z - p.z) * divisorInv;
    weights.value2 = (p.x - pMin.x) * (p.y - pMin.y) * (pMax.z - p.z) * divisorInv;
    weights.value3 = (p.x - pMin.x) * (pMax.y - p.y) * (pMax.z - p.z) * divisorInv;
    weights.value4 = (pMax.x - p.x) * (pMax.y - p.y) * (p.z - pMin.z) * divisorInv;
    weights.value5 = (pMax.x - p.x) * (p.y - pMin.y) * (p.z - pMin.z) * divisorInv;
    weights.value6 = (p.x - pMin.x) * (p.y - pMin.y) * (p.z - pMin.z) * divisorInv;
    weights.value7 = (p.x - pMin.x) * (pMax.y - p.y) * (p.z - pMin.z) * divisorInv;

    return weights;
}

// Fetches neighboring probes for some surface and interpolates their spherical harmonics
// based on surface's position and it's occlusion information
SH TriLerpSurroundingProbes(usampler3D gridSHMap0, // Each uint sampled from these textures
                            usampler3D gridSHMap1, // contains 2 encoded half-precision float values
                            usampler3D gridSHMap2, // 4 3D textures
                            usampler3D gridSHMap3,
                            samplerBuffer probeWorldPositions,
                            vec3 surfaceNormal,
                            vec3 surfaceWorldPosition,
                            // Transformation matrix to convert any world coordinates into normalized
                            // coordinates inside light probe grid. World position exactly at the center
                            // of the grid must transform to [0.5; 0.5; 0.5] and [0.0; 0.0; 0.0], [1.0; 1.0; 1.0]
                            // at the respective corners
                            mat4 gridSpaceTransform)
{
    ivec3 gridSize = textureSize(gridSHMap0, 0);

    // Compute normalized 3D texture coordinates of a surface
    vec3 normTexCoords = (gridSpaceTransform * vec4(surfaceWorldPosition, 1.0)).xyz;

    // Compute unnormalized coordinates of a surface, basically floating-point indices inside the probe grid
    vec3 unnormTexCoords = normTexCoords * vec3(gridSize - 1);

    // Get unnormalized texture coordinates of the closest corner points
    // that can represent a cell containing current surface in the light probe grid
    //
    vec3 minTexCoords = floor(unnormTexCoords); // index 0 on the "image"
    vec3 maxTexCoords = ceil(unnormTexCoords); // index 6 on the "image"

    //
    //        5-------6
    //       /|      /|
    // Y    / |     / |
    // ^   1--|----2  |
    // |   |  4----|--7
    // |   | /     | /
    // |   0-------3
    // |
    //  -----------> X
    //

    // Construct all 8 corner points for the current cell
    // (We're still using unnormalized texture coordinates)
    //
    vec3 cp0 = vec3(minTexCoords.x, minTexCoords.y, minTexCoords.z); vec3 cp1 = vec3(minTexCoords.x, maxTexCoords.y, minTexCoords.z);
    vec3 cp2 = vec3(maxTexCoords.x, maxTexCoords.y, minTexCoords.z); vec3 cp3 = vec3(maxTexCoords.x, minTexCoords.y, minTexCoords.z);
    vec3 cp4 = vec3(minTexCoords.x, minTexCoords.y, maxTexCoords.z); vec3 cp5 = vec3(minTexCoords.x, maxTexCoords.y, maxTexCoords.z);
    vec3 cp6 = vec3(maxTexCoords.x, maxTexCoords.y, maxTexCoords.z); vec3 cp7 = vec3(maxTexCoords.x, minTexCoords.y, maxTexCoords.z);

    ivec3 icp0 = ivec3(cp0); ivec3 icp1 = ivec3(cp1);
    ivec3 icp2 = ivec3(cp2); ivec3 icp3 = ivec3(cp3);
    ivec3 icp4 = ivec3(cp4); ivec3 icp5 = ivec3(cp5);
    ivec3 icp6 = ivec3(cp6); ivec3 icp7 = ivec3(cp7);

    SH sh0 = UnpackSH(gridSHMap0, gridSHMap1, gridSHMap2, gridSHMap3, icp0);
    SH sh1 = UnpackSH(gridSHMap0, gridSHMap1, gridSHMap2, gridSHMap3, icp1);
    SH sh2 = UnpackSH(gridSHMap0, gridSHMap1, gridSHMap2, gridSHMap3, icp2);
    SH sh3 = UnpackSH(gridSHMap0, gridSHMap1, gridSHMap2, gridSHMap3, icp3);
    SH sh4 = UnpackSH(gridSHMap0, gridSHMap1, gridSHMap2, gridSHMap3, icp4);
    SH sh5 = UnpackSH(gridSHMap0, gridSHMap1, gridSHMap2, gridSHMap3, icp5);
    SH sh6 = UnpackSH(gridSHMap0, gridSHMap1, gridSHMap2, gridSHMap3, icp6);
    SH sh7 = UnpackSH(gridSHMap0, gridSHMap1, gridSHMap2, gridSHMap3, icp7);

    float probe0OcclusionFactor = ProbeOcclusionFactor(probeWorldPositions, icp0, gridSize, surfaceNormal, surfaceWorldPosition);
    float probe1OcclusionFactor = ProbeOcclusionFactor(probeWorldPositions, icp1, gridSize, surfaceNormal, surfaceWorldPosition);
    float probe2OcclusionFactor = ProbeOcclusionFactor(probeWorldPositions, icp2, gridSize, surfaceNormal, surfaceWorldPosition);
    float probe3OcclusionFactor = ProbeOcclusionFactor(probeWorldPositions, icp3, gridSize, surfaceNormal, surfaceWorldPosition);
    float probe4OcclusionFactor = ProbeOcclusionFactor(probeWorldPositions, icp4, gridSize, surfaceNormal, surfaceWorldPosition);
    float probe5OcclusionFactor = ProbeOcclusionFactor(probeWorldPositions, icp5, gridSize, surfaceNormal, surfaceWorldPosition);
    float probe6OcclusionFactor = ProbeOcclusionFactor(probeWorldPositions, icp6, gridSize, surfaceNormal, surfaceWorldPosition);
    float probe7OcclusionFactor = ProbeOcclusionFactor(probeWorldPositions, icp7, gridSize, surfaceNormal, surfaceWorldPosition);

    // Get interpolation weights
    vec8 weights = TriLerp(minTexCoords, maxTexCoords, unnormTexCoords);

    float excludedWeight = 0.0;

    // Apply occlusion factors to weights and remember how much weight was excluded
    excludedWeight += weights.value0 * (1.0 - probe0OcclusionFactor);
    weights.value0 *= probe0OcclusionFactor;

    excludedWeight += weights.value1 * (1.0 - probe1OcclusionFactor);
    weights.value1 *= probe1OcclusionFactor;

    excludedWeight += weights.value2 * (1.0 - probe2OcclusionFactor);
    weights.value2 *= probe2OcclusionFactor;

    excludedWeight += weights.value3 * (1.0 - probe3OcclusionFactor);
    weights.value3 *= probe3OcclusionFactor;

    excludedWeight += weights.value4 * (1.0 - probe4OcclusionFactor);
    weights.value4 *= probe4OcclusionFactor;

    excludedWeight += weights.value5 * (1.0 - probe5OcclusionFactor);
    weights.value5 *= probe5OcclusionFactor;

    excludedWeight += weights.value6 * (1.0 - probe6OcclusionFactor);
    weights.value6 *= probe6OcclusionFactor;

    excludedWeight += weights.value7 * (1.0 - probe7OcclusionFactor);
    weights.value7 *= probe7OcclusionFactor;

    // Rescale interpolation weights to full strength.
    // The trick is that zero weights will remain zero but
    // non-zero weights will still sum up to 1.
    // By doing this we're effectively excluding occluded probes
    // from calculations
    float weightScale = 1.0 / (1.0 - excludedWeight);

    weights.value0 *= weightScale; weights.value1 *= weightScale;
    weights.value2 *= weightScale; weights.value3 *= weightScale;
    weights.value4 *= weightScale; weights.value5 *= weightScale;
    weights.value6 *= weightScale; weights.value7 *= weightScale;

    // Scale spherical harmonics
    sh0 = ScaleSH(sh0, vec3(weights.value0)); sh1 = ScaleSH(sh1, vec3(weights.value1));
    sh2 = ScaleSH(sh2, vec3(weights.value2)); sh3 = ScaleSH(sh3, vec3(weights.value3));
    sh4 = ScaleSH(sh4, vec3(weights.value4)); sh5 = ScaleSH(sh5, vec3(weights.value5));
    sh6 = ScaleSH(sh6, vec3(weights.value6)); sh7 = ScaleSH(sh7, vec3(weights.value7));

    // Then sum them together to obtain a final interpolated value for the current surface point
    SH result = Sum8SH(sh0, sh1, sh2, sh3, sh4, sh5, sh6, sh7);

    return result;
}

vec3 EvaluateDiffuseLightProbes(usampler3D gridSHMap0, // Each uint sampled from these textures
                                usampler3D gridSHMap1, // contains 2 encoded half-precision float values
                                usampler3D gridSHMap2, // 4 3D textures
                                usampler3D gridSHMap3,
                                samplerBuffer probeWorldPositions,
                                vec3 surfaceNormal,
                                vec3 surfaceWorldPosition,
                                mat4 gridSpaceTransform)
{
    SH sh = TriLerpSurroundingProbes(gridSHMap0,
                                     gridSHMap1,
                                     gridSHMap2,
                                     gridSHMap3,
                                     probeWorldPositions,
                                     surfaceNormal,
                                     surfaceWorldPosition,
                                     gridSpaceTransform);

    return vec3(EvaluateSH(sh, surfaceNormal, 0), EvaluateSH(sh, surfaceNormal, 1), EvaluateSH(sh, surfaceNormal, 2));
}
