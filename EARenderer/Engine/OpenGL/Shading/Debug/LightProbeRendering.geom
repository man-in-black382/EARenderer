#version 400 core

// Layout

layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

// Input

// Uniforms

uniform vec3 uCameraPosition;
uniform mat4 uCameraSpaceMat;
uniform mat4 uWorldBoudningBoxTransform;
uniform float uRadius;

// Outputs

out vec4 vCurrentPosition;
out vec3 vTexCoords;

// Functions

// Rotation matrix used to orient disk to the viewer
mat4 RotationMatrix(vec3 probePosition) {
    vec3 worldUp = vec3(0.0, 1.0, 0.0);
    vec3 zAxis = -normalize(uCameraPosition - probePosition);

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
    vec4 probePosition = gl_in[0].gl_Position;
    vec3 texCoords = (uWorldBoudningBoxTransform * probePosition).xyz;
    mat4 rotationMatrix = RotationMatrix(probePosition.xyz);

    vec4 a = vec4(-uRadius, -uRadius, 0.0, 0.0);
    vTexCoords = texCoords;
    vCurrentPosition = a;
    a = rotationMatrix * a;
    a = a + probePosition;
    a = uCameraSpaceMat * a;
    gl_Position = a;
    EmitVertex();

    vec4 b = vec4(-uRadius, uRadius, 0.0, 0.0);
    vTexCoords = texCoords;
    vCurrentPosition = b;
    b = rotationMatrix * b;
    b = b + probePosition;
    b = uCameraSpaceMat * b;
    gl_Position = b;
    EmitVertex();

    vec4 c = vec4(uRadius, -uRadius, 0.0, 0.0);
    vTexCoords = texCoords;
    vCurrentPosition = c;
    c = rotationMatrix * c;
    c = c + probePosition;
    c = uCameraSpaceMat * c;
    gl_Position = c;
    EmitVertex();

    vec4 d = vec4(uRadius, uRadius, 0.0, 0.0);
    vTexCoords = texCoords;
    vCurrentPosition = d;
    d = rotationMatrix * d;
    d = d + probePosition;
    d = uCameraSpaceMat * d;
    gl_Position = d;
    EmitVertex();

    EndPrimitive();
}

