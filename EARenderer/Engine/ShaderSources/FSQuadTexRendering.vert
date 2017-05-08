#version 400

uniform mat4 uProjectionMatrix;
uniform mat4 uViewMatrix;

out vec2 oTexCoords;

void main() {
    gl_Position = vec4(((gl_VertexID & 1) << 2) - 1, (gl_VertexID & 2) * 2 - 1, 1.0, 1.0);
    oTexCoords = vec2(gl_Position.xy / 2.0 + 0.5);
}

