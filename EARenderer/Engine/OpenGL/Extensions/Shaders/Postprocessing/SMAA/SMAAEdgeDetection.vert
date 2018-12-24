#version 400 core

#include "SMAADefines.glsl"
#define SMAA_INCLUDE_VS 1
#define SMAA_INCLUDE_PS 0
#include "SMAA.glsl"

out vec2 vTexCoords;
out float4 vOffset0;
out float4 vOffset1;
out float4 vOffset2;

const vec2 kVertices[4] = vec2[]
(
 vec2(-1.0,  1.0),
 vec2(-1.0, -1.0),
 vec2( 1.0,  1.0),
 vec2( 1.0, -1.0)
);

void main() {
    gl_Position = vec4(kVertices[gl_VertexID], -0.99, 1.0);
    vTexCoords = gl_Position.xy / 2.0 + 0.5;

    float4 offsets[3];
    SMAAEdgeDetectionVS(vTexCoords, offsets);
    vOffset0 = offsets[0];
    vOffset1 = offsets[1];
    vOffset2 = offsets[2];
}

