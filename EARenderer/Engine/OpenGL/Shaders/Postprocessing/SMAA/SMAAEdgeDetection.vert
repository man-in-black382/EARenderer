#version 400 core

#include "SMAADefines.glsl"
#define SMAA_INCLUDE_VS 1
//#define SMAA_INCLUDE_PS 0
#include "SMAA.glsl"

out vec2 vTexCoords;
out float4 vOffset0;
//out float4 vOffset1;
//out float4 vOffset2;

void main() {
    gl_Position = vec4(((gl_VertexID & 1) << 2) - 1, (gl_VertexID & 2) * 2 - 1, 0.999, 1.0);
    vTexCoords = gl_Position.xy / 2.0 + 0.5;
    
    float4 offsets[3];
    SMAAEdgeDetectionVS(vTexCoords, offsets);
    vOffset0 = offsets[0];
    vOffset1 = offsets[1];
    vOffset2 = offsets[2];
}

