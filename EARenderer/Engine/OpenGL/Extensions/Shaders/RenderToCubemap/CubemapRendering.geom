#version 400 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 18) out;

uniform mat4 uModelMat;
uniform mat4 uViewProjectionMatrices[6];

out vec4 vFragPosition;

void main() {
    for (int face = 0; face < 6; ++face) {
        for (int i = 0; i < gl_in.length(); i++) {
            // Cubemap face
            gl_Layer = face;
            gl_Position = gl_in[i].gl_Position;
            vFragPosition = uViewProjectionMatrices[face] * uModelMat * gl_Position;
            EmitVertex();
        }
        EndPrimitive();
    }
}

