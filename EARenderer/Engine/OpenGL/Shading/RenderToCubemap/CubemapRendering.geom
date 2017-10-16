#version 400 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 18) out;

uniform mat4 uViewProjectionMatrices[6];

out vec4 vFragPosition;
out vec2 vTexCoords;

void main() {
    for (int face = 0; face < 6; ++face) {
        // Cubemap face
        for (int i = 0; i < gl_in.length(); i++) {
            gl_Layer = face;
            gl_Position = gl_in[i].gl_Position;
            vFragPosition = uViewProjectionMatrices[face] * gl_Position;
            // Convert to [0; 1] range and invert to make origin be in the lower left corner
            vTexCoords = 1.0 - (gl_Position.xy + 1.0) / 2.0;
            EmitVertex();
        }
        EndPrimitive();
    }
}

