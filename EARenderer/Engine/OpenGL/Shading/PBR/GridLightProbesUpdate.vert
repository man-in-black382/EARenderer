#version 400 core

// Output

out InterfaceBlock {
    vec2 texCoords;
    int instanceID;
} vs_out;

// Functions

void main() {
    gl_Position = vec4(((gl_VertexID & 1) << 2) - 1, (gl_VertexID & 2) * 2 - 1, 0.99, 1.0);
    vs_out.texCoords = gl_Position.xy / 2.0 + 0.5;
    vs_out.instanceID = gl_InstanceID;
}
