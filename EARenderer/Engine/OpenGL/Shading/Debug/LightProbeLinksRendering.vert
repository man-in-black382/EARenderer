#version 400 core

// Attributes

layout (location = 0) in vec3 iPosition;

// Functions

void main() {
    gl_Position = vec4(iPosition, 1.0);
}
