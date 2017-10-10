#version 400 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 18) out;

uniform mat4 uViewProjectionMatrices[6];

out vec4 vFragPosition;

void main() {
    for (int face = 0; face < 6; ++face) {
        // Cubemap face
        gl_Layer = face;
        for (int i = 0; i < gl_in.length(); i++) {
            vFragPosition = gl_in[i].gl_Position;
            gl_Position = uViewProjectionMatrices[face] * vFragPosition;
            EmitVertex();
        }
        EndPrimitive();
    }
}

