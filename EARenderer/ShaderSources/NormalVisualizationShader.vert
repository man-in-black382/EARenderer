#version 400

layout (location = 0) in vec4 aPosition;

uniform mat4 uModelViewProjection;

void main() {
    gl_Position = uModelViewProjection * aPosition;
}
