#version 400 core

// Layout

layout (triangles) in;
layout (line_strip, max_vertices = 6) out;

void main() {
    vec3 side1 = gl_in[2].gl_Position.xyz - gl_in[0].gl_Position.xyz;
    vec3 side2 = gl_in[1].gl_Position.xyz - gl_in[0].gl_Position.xyz;
    vec3 normal = normalize(cross(side1, side2));
    vec4 displacement = vec4(normal * 0.0001, 0.0);
    
    gl_Position = gl_in[0].gl_Position + displacement;
    EmitVertex();
    
    gl_Position = gl_in[1].gl_Position + displacement;
    EmitVertex();
    
    gl_Position = gl_in[1].gl_Position + displacement;
    EmitVertex();
    
    gl_Position = gl_in[2].gl_Position + displacement;
    EmitVertex();
    
    gl_Position = gl_in[0].gl_Position + displacement;
    EmitVertex();
    
    gl_Position = gl_in[2].gl_Position + displacement;
    EmitVertex();

    EndPrimitive();
}

