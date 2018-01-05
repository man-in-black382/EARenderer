#version 400 core

// Layout

layout (points) in;
layout (points, max_vertices = 1) out;

in vec3 gNormal[];

void main() {
    vec4 displacement = vec4(gNormal[0] * 0.0001, 0.0);
    gl_Position = gl_in[0].gl_Position;// + displacement;
    gl_PointSize = 3.5;
    EmitVertex();
    EndPrimitive();
}

