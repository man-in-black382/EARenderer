#version 400 core

#include "SMAADefines.glsl"
#define SMAA_INCLUDE_VS 0
#define SMAA_INCLUDE_PS 1
#include "SMAA.glsl"

// Uniforms

uniform sampler2D uImage;
uniform sampler2D uBlendingWeights;

// Inputs

in vec2 vTexCoords;
in float4 vOffset;

// Output

out vec4 oOutput;

// Functions

void main() {
    oOutput = SMAANeighborhoodBlendingPS(vTexCoords, vOffset, uImage, uBlendingWeights);
}
