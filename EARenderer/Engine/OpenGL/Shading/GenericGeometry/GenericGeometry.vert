#version 400 core

layout (location = 0) in vec3 aPosition;

uniform mat4 uModelViewProjection;

void main() {
    gl_PointSize = 5.0;
    gl_Position = uModelViewProjection * vec4(aPosition, 1.0);
}
