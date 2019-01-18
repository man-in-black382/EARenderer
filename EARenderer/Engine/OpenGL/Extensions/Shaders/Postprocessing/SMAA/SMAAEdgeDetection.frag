#version 400 core

#include "SMAADefines.glsl"
#define SMAA_INCLUDE_VS 0
#define SMAA_INCLUDE_PS 1
#include "SMAA.glsl"

// Uniforms

uniform sampler2D uImage;

// Inputs

in vec2 vTexCoords;
in float4 vOffset0;
in float4 vOffset1;
in float4 vOffset2;

// Output

out vec2 oOutput;

// Functions

void main() {
    float4 offsets[3] = float4[3]( vOffset0, vOffset1, vOffset2 );
    oOutput = SMAALumaEdgeDetectionPS(vTexCoords, offsets, uImage);
//    oOutput = SMAAColorEdgeDetectionPS(vTexCoords, offsets, uImage);
}
