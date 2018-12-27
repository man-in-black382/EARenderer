#version 400 core

#include "Packing.glsl"
#include "ColorSpace.glsl"
#include "Sphericalharmonics.glsl"
#include "Constants.glsl"

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

// Functions

// Shrink tex coords by the size of 1 texel, which will result in a (0; 0; 0)
// coordinate to become (0.5; 0.5; 0.5) coordinate (in texel space)
vec3 AlignWithTexelCenters(vec3 texCoords) {
    vec3 halfTexel = 1.0 / vec3(uProbesGridResolution) / 2.0;
    vec3 reductionFactor = vec3(uProbesGridResolution - 1) / vec3(uProbesGridResolution);
    return texCoords * reductionFactor + halfTexel;
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

    return sh;
}

vec3 EvaluateSphericalHarmonics(vec3 direction) {
    SH sh = UnpackSH_333_HalfPacked();
    return vec3(EvaluateSH(sh, direction, 0), EvaluateSH(sh, direction, 1), EvaluateSH(sh, direction, 2));
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
    color = RGB_From_YCoCg(color);

    oFragColor = vec4(color, 1.0);
}
