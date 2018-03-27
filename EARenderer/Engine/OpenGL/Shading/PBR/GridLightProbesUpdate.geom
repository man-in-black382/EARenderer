#version 400 core

// Layout

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

// Uniforms

uniform int uProbesPerGridDimensionCount;

// Input

in InterfaceBlock {
    vec2 vTexCoords;
} gs_in[];

// Output

out vec3 vTexCoords;

void main() {
        for (int i = 0; i < gl_in.length(); i++) {
            // Cubemap face
            gl_Layer = gl_InstanceID;
            gl_Position = gl_in[i].gl_Position;
            vTexCoords = vec3(gs_in[i].vTexCoords, float(gl_Layer) / float(uProbesPerGridDimensionCount));
            EmitVertex();
        }
        EndPrimitive();
}

