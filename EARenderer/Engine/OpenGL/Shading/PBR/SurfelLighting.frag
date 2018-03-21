#version 400 core

// Constants

const float PI = 3.1415926535897932384626433832795;
const int kMaxCascades = 4;

const int kLightTypeDirectional = 0;
const int kLightTypePoint       = 1;
const int kLightTypeSpot        = 2;

// Spherical harmonics
const float kC1 = 0.429043;
const float kC2 = 0.511664;
const float kC3 = 0.743125;
const float kC4 = 0.886227;
const float kC5 = 0.247708;

// Output

out vec4 oFragColor;

// Input

in vec3 vTexCoords;

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

struct SH {
    vec3 L00;
    vec3 L11;
    vec3 L10;
    vec3 L1_1;
    vec3 L21;
    vec3 L2_1;
    vec3 L2_2;
    vec3 L20;
    vec3 L22;
};

uniform DirectionalLight uDirectionalLight;
uniform PointLight uPointLight;
uniform Spotlight uSpotlight;

uniform int uLightType;

// Shadow mapping
uniform sampler2DArray uShadowMapArray;
uniform float uDepthSplits[kMaxCascades];
uniform int uNumberOfCascades;

// Shperical harmonics

////////////////////////////////////////////////////////////
/////////////////// Spherical harmonics ////////////////////
////////////////////////////////////////////////////////////

//
// Unpacks spherical harmonics coefficients
// from the corresponding sample buffer
//
SH UnpackSH(int index) {
    SH sh;

    sh.L00 = vec3(texelFetch(uSphericalHarmonicsBuffer, index + 0).rgb);
    sh.L11 = vec3(texelFetch(uSphericalHarmonicsBuffer, index + 1).rgb);
    sh.L10 = vec3(texelFetch(uSphericalHarmonicsBuffer, index + 2).rgb);
    sh.L1_1 = vec3(texelFetch(uSphericalHarmonicsBuffer, index + 3).rgb);
    sh.L21 = vec3(texelFetch(uSphericalHarmonicsBuffer, index + 4).rgb);
    sh.L2_1 = vec3(texelFetch(uSphericalHarmonicsBuffer, index + 5).rgb);
    sh.L2_2 = vec3(texelFetch(uSphericalHarmonicsBuffer, index + 6).rgb);
    sh.L20 = vec3(texelFetch(uSphericalHarmonicsBuffer, index + 7).rgb);
    sh.L22 = vec3(texelFetch(uSphericalHarmonicsBuffer, index + 8).rgb);

    return sh;

//    return SH(vec3(1.0), vec3(1.0), vec3(1.0), vec3(1.0), vec3(1.0), vec3(1.0), vec3(1.0), vec3(1.0), vec3(1.0));
}

float SHRadiance(SH sh, vec3 direction, int component) {
    int c = component;

    return  kC1 * sh.L22[c] * (direction.x * direction.x - direction.y * direction.y) +
            kC3 * sh.L20[c] * (direction.z * direction.z) +
            kC4 * sh.L00[c] -
            kC5 * sh.L20[c] +
            2.0 * kC1 * (sh.L2_2[c] * direction.x * direction.y + sh.L21[c] * direction.x * direction.z + sh.L2_1[c] * direction.y * direction.z) +
            2.0 * kC2 * (sh.L11[c] * direction.x + sh.L1_1[c] * direction.y + sh.L10[c] * direction.z);
}

vec3 EvaluateSphericalHarmonics(vec3 direction) {
    SH sh = UnpackSH(0);
    return vec3(SHRadiance(sh, direction, 0), SHRadiance(sh, direction, 1), SHRadiance(sh, direction, 2));
}

////////////////////////////////////////////////////////////
//////////// Radiance of different light types /////////////
////////////////////////////////////////////////////////////

vec3 PointLightRadiance(vec3 N) {
    vec3 Wi                 = normalize(uPointLight.position - vWorldPosition);     // To light vector
    float distance          = length(Wi);                                           // Distance from fragment to light
    float attenuation       = 1.0 / (distance * distance);                          // How much enegry has light lost at current distance
    
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
    vec3 projCoords = vPosInCameraSpace.xyz / vPosInCameraSpace.w;
    // No need to transform to [0,1] range,
    // because splits passed from client are in [-1; 1]
    
    float fragDepth = projCoords.z;
    
    for (int i = 0; i < uNumberOfCascades; ++i) {
        if (fragDepth < uDepthSplits[i]) {
            return i;
        }
    }
}

float Shadow(in vec3 N, in vec3 L)
{
    int shadowCascadeIndex = shadowCascadeIndex();
    
    // perform perspective division
    vec3 projCoords = vPosInLightSpace[shadowCascadeIndex].xyz / vPosInLightSpace[shadowCascadeIndex].w;
    
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
    vec3 albedo             = FetchAlbedoMap();
    vec3 N                  = FetchNormalMap();
    vec3 L                  = vec3(0.0);
    vec3 radiance           = vec3(0.0);
    float shadow            = 0.0;

    // Analytical lighting
    
    if (uLightType == kLightTypeDirectional) {
        radiance    = DirectionalLightRadiance();
        L           = -normalize(uDirectionalLight.direction);
        shadow      = Shadow(N, L);
    } else if (uLightType == kLightTypePoint) {
        radiance    = PointLightRadiance(N);
        L           = normalize(uPointLight.position - vWorldPosition);
    } else if (uLightType == kLightTypeSpot) {
        // Nothing to do here... yet
    }

    vec3 NdotL              = dot(N, L);
    vec3 diffuseRadiance    = albedo / PI * NdotL;

    // Apply shadow factor
    diffuseRadiance *= 1.0 - shadow;

    oFragColor = vec4(diffuseRadiance, 1.0);
}
