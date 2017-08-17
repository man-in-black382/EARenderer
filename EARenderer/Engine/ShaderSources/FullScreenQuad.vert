#version 400 core

out vec2 oTexCoords;

void main() {
    gl_Position = vec4(((gl_VertexID & 1) << 2) - 1, (gl_VertexID & 2) * 2 - 1, 0.0, 1.0);
    oTexCoords = gl_Position.xy / 2.0 + 0.5;
}

