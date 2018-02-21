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
out vec4 iCurrentPosition;

// Rotation matrix used to orient surfel disk around surfel's normal
mat4 RotationMatrix() {
    vec3 worldUp = vec3(0.0, -1.0, 0.0);
    vec3 zAxis = gNormal[0];
    vec3 xAxis = normalize(cross(worldUp, zAxis));
    vec3 yAxis = cross(zAxis, xAxis);

    return mat4(vec4(xAxis, 0.0),
                vec4(yAxis, 0.0),
                vec4(zAxis, 0.0),
                vec4(0.0, 0.0, 0.0, 1.0));
}

void main() {
    mat4 rotationMatrix = RotationMatrix();
    vec4 surfelPosition = gl_in[0].gl_Position;

    vec4 a = vec4(-uRadius, -uRadius, 0.0, 1.0);
    iCurrentPosition = a;
    a = rotationMatrix * a;
    a = a + surfelPosition;
    a = uViewProjectionMatrix * a;
    gl_Position = a;
    EmitVertex();

    vec4 b = vec4(-uRadius, uRadius, 0.0, 1.0);
    iCurrentPosition = b;
    b = rotationMatrix * b;
    b = b + surfelPosition;
    b = uViewProjectionMatrix * b;
    gl_Position = b;
    EmitVertex();

    vec4 c = vec4(uRadius, -uRadius, 0.0, 1.0);
    iCurrentPosition = c;
    c = rotationMatrix * c;
    c = c + surfelPosition;
    c = uViewProjectionMatrix * c;
    gl_Position = c;
    EmitVertex();

    vec4 d = vec4(uRadius, uRadius, 0.0, 1.0);
    iCurrentPosition = d;
    d = rotationMatrix * d;
    d = d + surfelPosition;
    d = uViewProjectionMatrix * d;
    gl_Position = d;
    EmitVertex();

    EndPrimitive();
}

