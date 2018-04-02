#version 400 core

// Constants

// Spherical harmonics
const float kC1 = 0.429043;
const float kC2 = 0.511664;
const float kC3 = 0.743125;
const float kC4 = 0.886227;
const float kC5 = 0.247708;

// Input

in vec4 vCurrentPosition;
in vec3 vTexCoords;
in vec3 vNormal;

// Output

out vec4 oFragColor;

// Uniforms

uniform float uRadius;

uniform sampler3D uGridSHMap0;
uniform sampler3D uGridSHMap1;
uniform sampler3D uGridSHMap2;
uniform sampler3D uGridSHMap3;
uniform sampler3D uGridSHMap4;
uniform sampler3D uGridSHMap5;
uniform sampler3D uGridSHMap6;

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

SH UnpackSH() {
    SH sh;

    vec3 shMapCoords = vTexCoords;

    vec4 shMap0Data = texture(uGridSHMap0, shMapCoords);
    vec4 shMap1Data = texture(uGridSHMap1, shMapCoords);
    vec4 shMap2Data = texture(uGridSHMap2, shMapCoords);
    vec4 shMap3Data = texture(uGridSHMap3, shMapCoords);
    vec4 shMap4Data = texture(uGridSHMap4, shMapCoords);
    vec4 shMap5Data = texture(uGridSHMap5, shMapCoords);
    vec4 shMap6Data = texture(uGridSHMap6, shMapCoords);

    sh.L00  = vec3(shMap0Data.rgb);
    sh.L11  = vec3(shMap0Data.a, shMap1Data.rg);
    sh.L10  = vec3(shMap1Data.ba, shMap2Data.r);
    sh.L1_1 = vec3(shMap2Data.gba);
    sh.L21  = vec3(shMap3Data.rgb);
    sh.L2_1 = vec3(shMap3Data.a, shMap4Data.rg);
    sh.L2_2 = vec3(shMap4Data.ba, shMap5Data.r);
    sh.L20  = vec3(shMap5Data.gba);
    sh.L22  = vec3(shMap6Data.rgb);

    return sh;
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
    SH sh = UnpackSH();
    return vec3(SHRadiance(sh, direction, 0), SHRadiance(sh, direction, 1), SHRadiance(sh, direction, 2));
}

// https://stackoverflow.com/a/10506172/4308277

void main() {
    // Draw a sphere
    // Don't forget to exclude W component from length calculation
    float distanceFromCenter = length(vCurrentPosition.xy);

    if (distanceFromCenter > uRadius) {
        discard;
    }

    float normalizedDepth = sqrt(1.0 - distanceFromCenter * distanceFromCenter);
    // Calculate the lighting normal for the sphere
    vec3 normal = normalize(vec3(vCurrentPosition.xy, normalizedDepth));

    oFragColor = vec4(EvaluateSphericalHarmonics(normal), 1.0);
}
