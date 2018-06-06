#version 400 core

// Layout

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

// Input

in InterfaceBlock {
    vec2 texCoords;
    int instanceID;
} gs_in[];

// Output

out vec2 vTexCoords;
out float vLayer;

void main() {
    for (int i = 0; i < gl_in.length(); i++) {
        gl_Layer = gs_in[i].instanceID;
        gl_Position = gl_in[i].gl_Position;
        vTexCoords = gs_in[i].texCoords;
        vLayer = float(gl_Layer);
        EmitVertex();
    }
    EndPrimitive();
}

