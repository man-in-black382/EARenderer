#version 400 core

#include "Packing.glsl"
#include "ColorSpace.glsl"

// Constants
const float PI = 3.1415926535897932384626433832795;

// Spherical harmonics
const float Y00 = 0.28209479177387814347; // 1 / (2*sqrt(pi))
const float Y11 = -0.48860251190291992159; // sqrt(3 /(4pi))
const float Y10 = 0.48860251190291992159;
const float Y1_1 = -0.48860251190291992159;
const float Y21 = -1.09254843059207907054; // 1 / (2*sqrt(pi))
const float Y2_1 = -1.09254843059207907054;
const float Y2_2 = 1.09254843059207907054;
const float Y20 = 0.31539156525252000603; // 1/4 * sqrt(5/pi)
const float Y22 = 0.54627421529603953527; // 1/4 * sqrt(15/pi)

// Input

in vec3 vCurrentPosition;
in vec3 vTexCoords;
in mat3 vNormalMatrix;

// Output

out vec4 oFragColor;

// Uniforms

uniform float uRadius;
uniform ivec3 uProbesGridResolution;

uniform usampler3D uGridSHMap0;
uniform usampler3D uGridSHMap1;
uniform usampler3D uGridSHMap2;
uniform usampler3D uGridSHMap3;

// Types

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

// Functions

// Shrink tex coords by the size of 1 texel, which will result in a (0; 0; 0)
// coordinate to become (0.5; 0.5; 0.5) coordinate (in texel space)
vec3 AlignWithTexelCenters(vec3 texCoords) {
    vec3 halfTexel = 1.0 / vec3(uProbesGridResolution) / 2.0;
    vec3 reductionFactor = vec3(uProbesGridResolution - 1) / vec3(uProbesGridResolution);
    return texCoords * reductionFactor + halfTexel;
}

SH ZeroSH() {
    SH result;
    result.L00  = vec3(0.0); result.L1_1 = vec3(0.0); result.L10  = vec3(0.0);
    result.L11  = vec3(0.0); result.L2_2 = vec3(0.0); result.L2_1 = vec3(0.0);
    result.L21  = vec3(0.0); result.L20  = vec3(0.0); result.L22  = vec3(0.0);
    return result;
}

SH ScaleSH(SH sh, vec3 scale) {
    SH result;
    result.L00  = scale * sh.L00; result.L1_1 = scale * sh.L1_1; result.L10  = scale * sh.L10;
    result.L11  = scale * sh.L11; result.L2_2 = scale * sh.L2_2; result.L2_1 = scale * sh.L2_1;
    result.L21  = scale * sh.L21; result.L20  = scale * sh.L20;  result.L22  = scale * sh.L22;
    return result;
}

SH UnpackSH_311_HalfPacked() {
    SH sh = ZeroSH();

    ivec3 iTexCoords = ivec3(AlignWithTexelCenters(vTexCoords) * uProbesGridResolution);

    uvec4 shMap0Data = texelFetch(uGridSHMap0, iTexCoords, 0);
    uvec4 shMap1Data = texelFetch(uGridSHMap1, iTexCoords, 0);

    float range = uintBitsToFloat(shMap1Data.a);

    vec2 pair0 = UnpackSnorm2x16(shMap0Data.r, range); vec2 pair1 = UnpackSnorm2x16(shMap0Data.g, range);
    vec2 pair2 = UnpackSnorm2x16(shMap0Data.b, range); vec2 pair3 = UnpackSnorm2x16(shMap0Data.a, range);
    vec2 pair4 = UnpackSnorm2x16(shMap1Data.r, range); vec2 pair5 = UnpackSnorm2x16(shMap1Data.g, range);

    // Y
    sh.L00.r  = pair0.x;  sh.L11.r  = pair0.y;  sh.L10.r  = pair1.x;
    sh.L1_1.r = pair1.y;  sh.L21.r  = pair2.x;  sh.L2_1.r = pair2.y;
    sh.L2_2.r = pair3.x;  sh.L20.r  = pair3.y;  sh.L22.r  = pair4.x;

    // Co, Cg
    sh.L00.g  = pair4.y; sh.L00.b  = pair5.x;

    return sh;
}

SH UnpackSH_333_HalfPacked() {
    SH sh = ZeroSH();

    ivec3 iTexCoords = ivec3(AlignWithTexelCenters(vTexCoords) * uProbesGridResolution);

    uvec4 shMap0Data = texelFetch(uGridSHMap0, iTexCoords, 0);
    uvec4 shMap1Data = texelFetch(uGridSHMap1, iTexCoords, 0);
    uvec4 shMap2Data = texelFetch(uGridSHMap2, iTexCoords, 0);
    uvec4 shMap3Data = texelFetch(uGridSHMap3, iTexCoords, 0);

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

    /////////
//    sh.L00  = vec3(5.56832886, 11.1366577, 5.56832886);
//    sh.L11  = vec3(-6.42975139, 0.0, -6.42975139);
//    sh.L10  = vec3(0.0);
//    sh.L1_1 = vec3(0.0);
//    sh.L21  = vec3(0.0);
//    sh.L2_1 = vec3(0.0);
//    sh.L2_2 = vec3(0.0);
//    sh.L20  = vec3(-1.55639505, -3.11279011, -1.55639505);
//    sh.L22  = vec3(2.69575548, 5.39151096, 2.69575548);
//
//    sh = ScaleSH(sh, vec3(50.0));
    /////////

    return sh;
}

float SHRadiance(SH sh, vec3 direction, int component) {
    int c = component;
    float result = 0.0;

    result += sh.L00[c] * Y00;

    result += sh.L1_1[c] * Y1_1 * direction.y;
    result += sh.L10[c] * Y10 * direction.z;
    result += sh.L11[c] * Y11 * direction.x;

    result += sh.L2_2[c] * Y2_2 * (direction.x * direction.y);
    result += sh.L2_1[c] * Y2_1 * (direction.y * direction.z);
    result += sh.L21[c] * Y21 * (direction.x * direction.z);
    result += sh.L20[c] * Y20 * (3.0f * direction.z * direction.z - 1.0f);
    result += sh.L22[c] * Y22 * (direction.x * direction.x - direction.y * direction.y);

    return result;
}

vec3 EvaluateSphericalHarmonics(vec3 direction) {
    SH sh = UnpackSH_333_HalfPacked();
    return vec3(SHRadiance(sh, direction, 0), SHRadiance(sh, direction, 1), SHRadiance(sh, direction, 2));
}

vec3 ReinhardToneMap(vec3 color) {
    return color / (color + vec3(1.0));
}

// Drawing a sphere using billboard
void main() {
    vec2 normPosition = vCurrentPosition.xy / uRadius;
    float normDistanceFromCenter = length(normPosition);

    if (normDistanceFromCenter > 1.0) {
        discard;
    }

    // Knowing that our billboard represents a sphere we can reconstruct the z coordinate
    // Also negate Z because the equation gives positive values for Z pointing "from the screen to the viewer"
    // which is the opposite of the positive Z direction in NDC space
    float z = -sqrt(1.0 - normDistanceFromCenter * normDistanceFromCenter);

    // Calculate the normal for the sphere
    vec3 normal = normalize(vec3(normPosition, z));

    // Rotate
    normal = vNormalMatrix * normal;

    vec3 color = EvaluateSphericalHarmonics(normal);
    color = max(vec3(0.0), color);
    color = RGB_From_YCoCg(color);

    oFragColor = vec4(color, 1.0);
}
