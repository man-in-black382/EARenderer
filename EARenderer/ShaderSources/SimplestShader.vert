#version 400

layout (location = 0) in vec4 a_position;
layout (location = 1) in vec2 a_texCoords;
layout (location = 2) in vec3 a_normal;

uniform mat4 u_mvp;

void main() {
    gl_Position = u_mvp * a_position;
}
