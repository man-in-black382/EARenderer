#version 400 core

// Attributes

layout (location = 0) in vec3 iPosition;

// Output

out InterfaceBlock {
    int instanceID;
} vs_out;

// Functions

void main() {
    gl_Position = vec4(iPosition, 1.0);
    vs_out.instanceID = gl_InstanceID;
}
