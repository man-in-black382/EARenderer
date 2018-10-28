#version 400 core

#include "Packing.glsl"
#include "DiffuseLightProbes.glsl"
#include "ColorSpace.glsl"

// Constants

const float PI = 3.1415926535897932384626433832795;
const int kMaxCascades = 4;

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

struct DirectionalLight {
    vec3 radiantFlux; // a.k.a color
    vec3 direction;
};

struct PointLight {
    vec3 radiantFlux; // a.k.a color
    vec3 position;
};

struct Spotlight {
    vec3 radiantFlux; // a.k.a color
    vec3 position;
    vec3 direction;
    float cutOffAngleCos;
};

uniform DirectionalLight uDirectionalLight;
uniform PointLight uPointLight;
uniform Spotlight uSpotlight;
uniform sampler2DArray uSurfelsGBuffer;

uniform int uLightType;
uniform bool uEnableMultibounce;

// Shadow mapping
uniform mat4 uCSMSplitSpaceMat;
uniform mat4 uLightSpaceMatrices[kMaxCascades];
uniform int uDepthSplitsAxis;
uniform float uDepthSplits[kMaxCascades];
uniform int uNumberOfCascades;
uniform float uESMFactor;
uniform sampler2D uDirectionalShadowMap;
uniform samplerCubeArray uOmnidirectionalShadowMaps;
uniform int uOmnidirectionalShadowMapIndex;

// Spherical harmonics
uniform usampler3D uGridSHMap0;
uniform usampler3D uGridSHMap1;
uniform usampler3D uGridSHMap2;
uniform usampler3D uGridSHMap3;

uniform samplerBuffer uProbePositions;
uniform mat4 uWorldBoudningBoxTransform;

////////////////////////////////////////////////////////////
//////////// Radiance of different light types /////////////
////////////////////////////////////////////////////////////

vec3 PointLightRadiance(vec3 N, vec3 position) {
    vec3 Wi                 = uPointLight.position - position;  // To light vector
    float distance          = length(Wi);                       // Distance from fragment to light
    float attenuation       = 1.0 / (distance * distance);      // How much enegry has light lost at current distance

    return uPointLight.radiantFlux * attenuation;
}

vec3 DirectionalLightRadiance() {
    return uDirectionalLight.radiantFlux;
}

////////////////////////////////////////////////////////////
///////////////////////// Shadows //////////////////////////
////////////////////////////////////////////////////////////

int ShadowCascadeIndex(vec3 worldPosition) {

    vec4 posInCSMSplitSpace = uCSMSplitSpaceMat * vec4(worldPosition, 1.0);

    vec3 projCoords = posInCSMSplitSpace.xyz / posInCSMSplitSpace.w;
    // No need to transform to [0,1] range,
    // because splits passed from client are in [-1; 1]

    float locationOnSplitAxis = projCoords[uDepthSplitsAxis];

    for (int i = 0; i < kMaxCascades; ++i) {
        if (locationOnSplitAxis < uDepthSplits[i]) {
            return i;
        }
    }
}

float DirectionalExponentialShadow(vec3 worldPosition) {
    int cascade = ShadowCascadeIndex(worldPosition);
    mat4 relevantLightMatrix = uLightSpaceMatrices[cascade];
    vec4 positionInLightSpace = relevantLightMatrix * vec4(worldPosition, 1.0);

    vec3 projCoords = positionInLightSpace.xyz / positionInLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

    // Linear Z a.k.a disntance from light
    // Distance to the fragment from a near clip plane of directional light's frustum
    float linearZ = positionInLightSpace.z * 0.5 + 0.5; // Transform to [0; 1] range

    // Explicitly reading from 0 LOD because shadow map comes from a postprocess texture pool
    // and is a subject to mipmapping
    vec4 occluders = textureLod(uDirectionalShadowMap, projCoords.xy, 0);

    float occluder = occluders[cascade];
    float receiver = exp(-uESMFactor * linearZ);
    float visibility = clamp(occluder * receiver, 0.0, 1.0);

    return visibility;
}

////////////////////////////////////////////////////////////
////////////////////////// Main ////////////////////////////
////////////////////////////////////////////////////////////

void main() {
    vec3 position  = texelFetch(uSurfelsGBuffer, ivec3(ivec2(gl_FragCoord.xy), int(kGBufferIndexPosition)), 0).rgb;
    vec3 N         = texelFetch(uSurfelsGBuffer, ivec3(ivec2(gl_FragCoord.xy), int(kGBufferIndexNormal)), 0).rgb;
    vec3 L         = vec3(0.0);
    vec3 radiance  = vec3(0.0);
    float shadow   = 0.0;

    // Analytical lighting

    if (uLightType == kLightTypeDirectional) {
        radiance    = DirectionalLightRadiance();
        L           = -normalize(uDirectionalLight.direction);
        shadow      = DirectionalExponentialShadow(position);
    } else if (uLightType == kLightTypePoint) {
        radiance    = PointLightRadiance(N, position);
        L           = normalize(uPointLight.position - position);
    } else if (uLightType == kLightTypeSpot) {
        // Nothing to do here... yet
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
                                                           position,
                                                           uWorldBoudningBoxTransform);

        indirectRadiance = RGB_From_YCoCg(indirectRadiance);
        indirectRadiance = max(vec3(0.0), indirectRadiance);

        finalColor += indirectRadiance;
    }

    oLuminance = LuminanceFromRGB(finalColor);
}
