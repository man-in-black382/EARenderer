#version 400 core

// Output

out InterfaceBlock {
    vec2 vTexCoords;
} vs_out;

// Functions

void main() {
    gl_Position = vec4(((gl_VertexID & 1) << 2) - 1, (gl_VertexID & 2) * 2 - 1, 0.99, 1.0);
    vs_out.vTexCoords = gl_Position.xy / 2.0 + 0.5;
}
