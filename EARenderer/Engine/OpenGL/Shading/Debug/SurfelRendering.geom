#version 400 core

// Layout

layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

// Input

in vec3 gNormal[];
in float gArea[];

// Uniforms

uniform mat4 uViewProjectionMatrix;
uniform float uRadius;

// Outputs
out vec4 iCenter;
out vec4 iCurrent;

// Rotation matrix used to orient surfel disk around surfel's normal
mat4 TBN() {
    vec3 worldUp = vec3(0.0, 1.0, 0.0);
    vec3 zAxis = normalize(gNormal[0]);
    vec3 xAxis = normalize(cross(worldUp, zAxis));
    vec3 yAxis = normalize(cross(zAxis, xAxis));

    return mat4(vec4(xAxis, 0.0),
                vec4(yAxis, 0.0),
                vec4(zAxis, 0.0),
                vec4(0.0, 0.0, 0.0, 1.0));
}

void main() {
//    mat4 rotationMatrix = mat4(TBN());
    mat4 rotationMatrix = mat4(1.0);

    vec4 a = gl_in[0].gl_Position;
    iCenter = a;
    a.x -= uRadius;
    a.y -= uRadius;
//    a.z = 0.0;
    iCurrent = a;
    gl_Position = uViewProjectionMatrix * rotationMatrix * a;
    EmitVertex();

    vec4 b = gl_in[0].gl_Position;
    iCenter = b;
    b.x -= uRadius;
    b.y += uRadius;
//    b.z = 0.0;
    iCurrent = b;
    gl_Position = uViewProjectionMatrix * rotationMatrix * b;
    EmitVertex();

    vec4 c = gl_in[0].gl_Position;
    iCenter = c;
    c.x += uRadius;
    c.y -= uRadius;
//    c.z = 0.0;
    iCurrent = c;
    gl_Position = uViewProjectionMatrix * rotationMatrix * c;
    EmitVertex();

    vec4 d = gl_in[0].gl_Position;
    iCenter = d;
    d.x += uRadius;
    d.y += uRadius;
//    d.z = 0.0;
    iCurrent = d;
    gl_Position = uViewProjectionMatrix * rotationMatrix * d;
    EmitVertex();

    EndPrimitive();

//    gl_Position = uViewProjectionMatrix * gl_in[0].gl_Position;
//    EmitVertex();
//
//    gl_Position = uViewProjectionMatrix * (gl_in[0].gl_Position + vec4(gNormal[0], 0.0) * vec4(0.1));
//    EmitVertex();
//
//    EndPrimitive();

}

