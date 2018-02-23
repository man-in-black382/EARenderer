#version 400 core

// Layout

layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

// Input

in vec3 gNormal[];
in vec3 gAlbedo[];
in float gArea[];

// Uniforms

uniform mat4 uViewProjectionMatrix;
uniform float uRadius;

// Outputs
out vec4 vCurrentPosition;
out vec3 vAlbedo;

// Rotation matrix used to orient surfel disk around surfel's normal
mat4 RotationMatrix() {
    vec3 worldUp = vec3(0.0, 1.0, 0.0);
    vec3 zAxis = -gNormal[0];

    if (abs(zAxis.x) < 0.000001 && abs(zAxis.z) < 0.000001) {
        worldUp = vec3(0.0, 0.0, 1.0);
    }
  
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
    float zDisplacement = -uRadius / 10.0;

    vec4 a = vec4(-uRadius, -uRadius, zDisplacement, 0.0);
    vCurrentPosition = a;
    vAlbedo = gAlbedo[0];
    a = rotationMatrix * a;
    a = a + surfelPosition;
    a = uViewProjectionMatrix * a;
    gl_Position = a;
    EmitVertex();

    vec4 b = vec4(-uRadius, uRadius, zDisplacement, 0.0);
    vCurrentPosition = b;
    vAlbedo = gAlbedo[0];
    b = rotationMatrix * b;
    b = b + surfelPosition;
    b = uViewProjectionMatrix * b;
    gl_Position = b;
    EmitVertex();

    vec4 c = vec4(uRadius, -uRadius, zDisplacement, 0.0);
    vCurrentPosition = c;
    vAlbedo = gAlbedo[0];
    c = rotationMatrix * c;
    c = c + surfelPosition;
    c = uViewProjectionMatrix * c;
    gl_Position = c;
    EmitVertex();

    vec4 d = vec4(uRadius, uRadius, zDisplacement, 0.0);
    vCurrentPosition = d;
    vAlbedo = gAlbedo[0];
    d = rotationMatrix * d;
    d = d + surfelPosition;
    d = uViewProjectionMatrix * d;
    gl_Position = d;
    EmitVertex();

    EndPrimitive();
}

