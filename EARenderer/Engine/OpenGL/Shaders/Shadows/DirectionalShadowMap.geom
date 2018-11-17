#version 400 core

#include "Constants.glsl"

// Layout

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

// Uniforms

uniform mat4 uModelMatrix;
uniform mat4 uLightSpaceMatrices[MaximumShadowCascadesCount];

// Input

in InterfaceBlock {
    int instanceID;
} gs_in[];

void main() {
    for (int i = 0; i < gl_in.length(); i++) {
        vec4 worldPosition = uModelMatrix * gl_in[i].gl_Position;
        vec4 lightSpacePosition = uLightSpaceMatrices[gs_in[i].instanceID] * worldPosition;
        
        gl_Layer = gs_in[i].instanceID;
        gl_Position = lightSpacePosition;
        
        EmitVertex();
    }
    EndPrimitive();
}
