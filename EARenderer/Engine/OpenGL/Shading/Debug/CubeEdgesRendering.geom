#version 400 core

// Layout

// Line means two points of axis-aligned box - min and max
layout (lines) in;
layout (line_strip, max_vertices = 18) out;

// Uniforms

uniform mat4 uViewProjectionMatrix;

// Functions

void main() {
    vec3 bMin = gl_in[0].gl_Position.xyz;
    vec3 bMax = gl_in[1].gl_Position.xyz;

    vec4 p0 = vec4( bMin.x, bMin.y, bMin.z, 1.0 ); // Bottom left front
    vec4 p1 = vec4( bMin.x, bMax.y, bMin.z, 1.0 ); // Top left front
    vec4 p2 = vec4( bMin.x, bMin.y, bMax.z, 1.0 ); // Bottom left back
    vec4 p3 = vec4( bMin.x, bMax.y, bMax.z, 1.0 ); // Top left back
    vec4 p4 = vec4( bMax.x, bMin.y, bMin.z, 1.0 ); // Bottom right front
    vec4 p5 = vec4( bMax.x, bMax.y, bMin.z, 1.0 ); // Top right front
    vec4 p6 = vec4( bMax.x, bMin.y, bMax.z, 1.0 ); // Bottom right back
    vec4 p7 = vec4( bMax.x, bMax.y, bMax.z, 1.0 ); // Top right back

    // Left side
    gl_Position = uViewProjectionMatrix * p0; // 1
    EmitVertex();

    gl_Position = uViewProjectionMatrix * p1; // 2
    EmitVertex();

    gl_Position = uViewProjectionMatrix * p3; // 3
    EmitVertex();

    gl_Position = uViewProjectionMatrix * p2; // 4
    EmitVertex();

    gl_Position = uViewProjectionMatrix * p0; // 5
    EmitVertex();

    EndPrimitive();

    // Right side
    gl_Position = uViewProjectionMatrix * p4; // 6
    EmitVertex();

    gl_Position = uViewProjectionMatrix * p5; // 7
    EmitVertex();

    gl_Position = uViewProjectionMatrix * p7; // 8
    EmitVertex();

    gl_Position = uViewProjectionMatrix * p6; // 9
    EmitVertex();

    gl_Position = uViewProjectionMatrix * p4; // 10
    EmitVertex();

    EndPrimitive();

    // Top edges
    gl_Position = uViewProjectionMatrix * p1; // 11
    EmitVertex();

    gl_Position = uViewProjectionMatrix * p5; // 12
    EmitVertex();

    EndPrimitive();

    gl_Position = uViewProjectionMatrix * p3; // 13
    EmitVertex();

    gl_Position = uViewProjectionMatrix * p7; // 14
    EmitVertex();

    EndPrimitive();

    // Bottom edges
    gl_Position = uViewProjectionMatrix * p0; // 11
    EmitVertex();

    gl_Position = uViewProjectionMatrix * p4; // 12
    EmitVertex();

    EndPrimitive();

    gl_Position = uViewProjectionMatrix * p2; // 13
    EmitVertex();

    gl_Position = uViewProjectionMatrix * p6; // 14
    EmitVertex();

    EndPrimitive();
}

