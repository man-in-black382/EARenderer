#version 400 core

// Constants

const float PI = 3.1415926535897932384626433832795;
const int kMaxCascades = 4;

const int kLightTypeDirectional = 0;
const int kLightTypePoint       = 1;
const int kLightTypeSpot        = 2;

const int kGBufferIndexPosition = 0;
const int kGBufferIndexNormal   = 1;
const int kGBufferIndexAlbedo   = 2;
const int kGBufferIndexUV       = 3;

// Output

out vec4 oFragColor;

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

// Shadow mapping
uniform sampler2DArray uShadowMapArray;
uniform float uDepthSplits[kMaxCascades];
uniform int uNumberOfCascades;
uniform mat4 uLightSpaceMatrices[kMaxCascades];

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

int shadowCascadeIndex()
{
//    vec3 projCoords = vPosInCameraSpace.xyz / vPosInCameraSpace.w;
//    // No need to transform to [0,1] range,
//    // because splits passed from client are in [-1; 1]
//
//    float fragDepth = projCoords.z;
//
//    for (int i = 0; i < uNumberOfCascades; ++i) {
//        if (fragDepth < uDepthSplits[i]) {
//            return i;
//        }
//    }

    return 0;
}

float Shadow(in vec3 N, in vec3 L, in vec3 worldPosition)
{
    int shadowCascadeIndex = shadowCascadeIndex();

    vec4 lightSpacePosition = uLightSpaceMatrices[shadowCascadeIndex] * vec4(worldPosition, 1.0);
    // perform perspective division
    vec3 projCoords = lightSpacePosition.xyz / lightSpacePosition.w;

    // Transformation to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;

    // Get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(uShadowMapArray, vec3(projCoords.xy, shadowCascadeIndex)).r;

    // Get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;

    // Check whether current frag pos is in shadow
    float bias = max(0.01 * (1.0 - dot(N, L)), 0.005);

    //    float shadow = 0.0;
    //    vec3 texelSize = 1.0 / textureSize(uShadowMapArray, 0);
    //    for(int x = -2; x <= 2; ++x) {
    //        for(int y = -2; y <= 2; ++y) {
    //            vec2 xy = projCoords.xy + vec2(x, y) * texelSize.xy;
    //            float pcfDepth = texture(uShadowMapArray, vec3(xy, shadowCascadeIndex)).r;
    //            float unbiasedDepth = currentDepth - bias;
    //            shadow += unbiasedDepth > pcfDepth && unbiasedDepth <= 1.0 ? 1.0 : 0.0;
    //        }
    //    }
    //    shadow /= 36.0;
    //    return shadow;

    float pcfDepth = texture(uShadowMapArray, vec3(projCoords.xy, shadowCascadeIndex)).r;
    float unbiasedDepth = currentDepth - bias;
    return unbiasedDepth > pcfDepth && unbiasedDepth <= 1.0 ? 1.0 : 0.0;
}

////////////////////////////////////////////////////////////
///////////////////////// Helpers //////////////////////////
////////////////////////////////////////////////////////////

vec3 ReinhardToneMapAndGammaCorrect(vec3 color) {
    vec3 mappedColor = color / (color + vec3(1.0));
    vec3 gammaCorrectedColor = pow(mappedColor, vec3(1.0 / 2.2));
    return gammaCorrectedColor;
}

vec3 ReinhardToneMap(vec3 color) {
    return color / (color + vec3(1.0));
}

vec3 GammaCorrect(vec3 color) {
    return pow(color, vec3(1.0 / 2.2));
}

vec3 LinearFromSRGB(vec3 sRGB) {
    return pow(sRGB, vec3(2.2));
}

////////////////////////////////////////////////////////////
////////////////////////// Main ////////////////////////////
////////////////////////////////////////////////////////////

void main() {
    int shadowMapsCount = uNumberOfCascades; // Stub to prevent from failing to compile on cpu side
    float split = uDepthSplits[0];  // Stub to prevent from failing to compile on cpu side

    vec3 position  = texture(uSurfelsGBuffer, vec3(vTexCoords, kGBufferIndexPosition)).rgb;
    vec3 albedo    = texture(uSurfelsGBuffer, vec3(vTexCoords, kGBufferIndexAlbedo)).rgb;
    vec3 N         = texture(uSurfelsGBuffer, vec3(vTexCoords, kGBufferIndexNormal)).rgb;
    vec3 L         = vec3(0.0);
    vec3 radiance  = vec3(0.0);
    float shadow   = 0.0;

    // Analytical lighting

    if (uLightType == kLightTypeDirectional) {
        radiance    = DirectionalLightRadiance();
        L           = -normalize(uDirectionalLight.direction);
        shadow      = Shadow(N, L, position);
    } else if (uLightType == kLightTypePoint) {
        radiance    = PointLightRadiance(N, position);
        L           = normalize(uPointLight.position - position);
    } else if (uLightType == kLightTypeSpot) {
        // Nothing to do here... yet
    }

    float NdotL             = dot(N, L);
    vec3 diffuseRadiance    = radiance * albedo / PI * NdotL;

    // Apply shadow factor
//    diffuseRadiance *= 1.0 - shadow;

    oFragColor = vec4(diffuseRadiance, 1.0);
}
