#version 400 core

#include "Constants.glsl"

// Inputs

layout (location = 0) in vec4 iPosition;

// Outputs

out InterfaceBlock {
    int instanceID;
} vs_out;

// Functions

void main() {
    vs_out.instanceID = gl_InstanceID;
    gl_Position = iPosition;
}
