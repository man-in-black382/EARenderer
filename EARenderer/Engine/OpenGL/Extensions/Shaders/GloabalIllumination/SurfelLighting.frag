#version 400 core

#include "Packing.glsl"
#include "DiffuseLightProbes.glsl"
#include "ColorSpace.glsl"
#include "DirectionalShadows.glsl"
#include "OmnidirectionalShadows.glsl"
#include "Constants.glsl"
#include "PointLightUBO.glsl"

// Constants

const int kLightTypeDirectional = 0;
const int kLightTypePoint       = 1;
const int kLightTypeSpot        = 2;

const int kGBufferIndexPosition = 0;
const int kGBufferIndexNormal   = 1;
const int kGBufferIndexAlbedo   = 2;

// Output

out float oLuminance;

// Input

in vec2 vTexCoords;

// Uniforms
uniform DirectionalLight uDirectionalLight;
uniform Spotlight uSpotlight;
uniform sampler2DArray uSurfelsGBuffer;

uniform int uLightType;
uniform bool uEnableMultibounce;

// Shadow mapping
uniform mat4 uCSMSplitSpaceMat;
uniform mat4 uLightSpaceMatrices[MaximumShadowCascadesCount];
uniform int uDepthSplitsAxis;
uniform float uDepthSplits[MaximumShadowCascadesCount];
uniform int uNumberOfCascades;
uniform float uESMFactor;
uniform sampler2DArrayShadow uDirectionalShadowMapArray;
uniform samplerCubeShadow uOmnidirectionalShadowMap;
uniform int uOmnidirectionalShadowMapIndex;

// Spherical harmonics
uniform usampler3D uGridSHMap0;
uniform usampler3D uGridSHMap1;
uniform usampler3D uGridSHMap2;
uniform usampler3D uGridSHMap3;

uniform samplerBuffer uProbePositions;
uniform mat4 uWorldBoudningBoxTransform;

////////////////////////////////////////////////////////////
////////////////////////// Main ////////////////////////////
////////////////////////////////////////////////////////////

void main() {
    vec3 worldPosition  = texelFetch(uSurfelsGBuffer, ivec3(ivec2(gl_FragCoord.xy), int(kGBufferIndexPosition)), 0).rgb;
    vec3 N              = texelFetch(uSurfelsGBuffer, ivec3(ivec2(gl_FragCoord.xy), int(kGBufferIndexNormal)), 0).rgb;
    vec3 L              = vec3(0.0);
    vec3 radiance       = vec3(0.0);
    float shadow        = 0.0;

    // Analytical lighting

    if (uLightType == kLightTypeDirectional) {
        radiance    = DirectionalLightRadiance(uDirectionalLight);
        L           = -normalize(uDirectionalLight.direction);
        int cascade = ShadowCascadeIndex(worldPosition, uCSMSplitSpaceMat, uDepthSplitsAxis, uDepthSplits);
        float penumbra = 5.0;
        shadow = DirectionalShadow(worldPosition, N, uDirectionalLight, cascade, uLightSpaceMatrices, uDirectionalShadowMapArray, penumbra);
    }
    else if (uLightType == kLightTypePoint) {
        radiance    = PointLightRadiance(uboPointLight, worldPosition);
        L           = normalize(uboPointLight.position.xyz - worldPosition);
        float penumbra = 5.0;
        shadow = OmnidirectionalShadow(worldPosition, N, uboPointLight, uOmnidirectionalShadowMap, penumbra);
    }

    float NdotL = max(dot(N, L), 0.0);

    // Surfel's color (albedo) is not needed here because it's already encoded
    // in the precomputed spherical harmonics.
    vec3 diffuseRadiance = radiance * NdotL;

    // Apply shadow factor
    diffuseRadiance *= shadow;

    vec3 finalColor = diffuseRadiance;

    if (uEnableMultibounce) {
        vec3 indirectRadiance = EvaluateDiffuseLightProbes(uGridSHMap0,
                                                           uGridSHMap1,
                                                           uGridSHMap2,
                                                           uGridSHMap3,
                                                           uProbePositions,
                                                           N,
                                                           worldPosition,
                                                           uWorldBoudningBoxTransform);

        indirectRadiance = RGB_From_YCoCg(indirectRadiance);
        indirectRadiance = max(vec3(0.0), indirectRadiance);

        finalColor += indirectRadiance;
    }

    oLuminance = LuminanceFromRGB(finalColor) / HDRNormalizationFactor; // Shrink the value so it wouldn't be clamped by additive blending. Restore in a later rendering stage.
}
