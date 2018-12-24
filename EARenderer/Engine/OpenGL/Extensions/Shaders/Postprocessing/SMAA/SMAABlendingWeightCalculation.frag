#version 400 core

#include "SMAADefines.glsl"
#define SMAA_INCLUDE_VS 0
#define SMAA_INCLUDE_PS 1
#include "SMAA.glsl"

// Uniforms

uniform sampler2D uEdgesTexture;
uniform sampler2D uAreaTexture;
uniform sampler2D uSearchTexture;

// Inputs

in vec2 vTexCoords;
in float4 vOffset0;
in float4 vOffset1;
in float4 vOffset2;

// Output

out vec4 oOutput;

// Functions

void main() {
    float4 offsets[3] = float4[3]( vOffset0, vOffset1, vOffset2 );

    vec2 texCoords = vec2(vTexCoords.x, vTexCoords.y);

    oOutput = SMAABlendingWeightCalculationPS(texCoords,
                                              gl_FragCoord.xy,
                                              offsets,
                                              uEdgesTexture,
                                              uAreaTexture,
                                              uSearchTexture,
                                              float4(0.0)); // Just pass zero for SMAA 1x, see @SUBSAMPLE_INDICES.
}
