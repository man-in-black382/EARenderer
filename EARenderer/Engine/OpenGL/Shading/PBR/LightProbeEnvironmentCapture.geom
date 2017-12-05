#version 400 core

// Layout

layout (triangles) in;
layout (triangle_strip, max_vertices = 18) out;

// Constaints

const int kMaxCascades = 4;

// Uniforms

uniform mat4 uViewProjectionMatrices[6];

// Input

in InterfaceBlock {
    vec3 vTexCoords;
    mat3 vTBN;
    vec4 vPosInLightSpace[kMaxCascades];
} gs_in[];

// Output

out vec3 vWorldPosition;
out vec3 vTexCoords;
out mat3 vTBN;
out vec4 vPosInLightSpace[kMaxCascades];
out vec4 vPosInCameraSpace;

void main() {
    for (int face = 0; face < 6; ++face) {
        for (int i = 0; i < gl_in.length(); i++) {
            // Cubemap face
            gl_Layer = face;
            gl_Position = uViewProjectionMatrices[face] * gl_in[i].gl_Position;
            
            vWorldPosition = gl_in[i].gl_Position.xyz;
            vPosInCameraSpace = gl_Position;
            vPosInLightSpace = gs_in[i].vPosInLightSpace;
            vTBN = gs_in[i].vTBN;
            vTexCoords = gs_in[i].vTexCoords;
            
            EmitVertex();
        }
        EndPrimitive();
    }
}

