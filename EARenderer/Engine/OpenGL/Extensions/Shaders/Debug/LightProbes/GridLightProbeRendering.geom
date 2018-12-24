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

out vec3 vCurrentPosition;
out vec3 vTexCoords;
out mat3 vNormalMatrix;

// Functions

// Rotation matrix used to orient disk to the viewer
mat4 RotationMatrix(vec3 probePosition) {
    vec3 worldUp = vec3(0.0, 1.0, 0.0);
    vec3 zAxis = normalize(probePosition - uCameraPosition);

    if (abs(zAxis.x) < 0.000001 && abs(zAxis.z) < 0.000001) {
        worldUp = vec3(0.0, 0.0, 1.0);
    }

    vec3 xAxis = normalize(cross(worldUp, zAxis));
    vec3 yAxis = cross(zAxis, xAxis);

    return mat4(vec4(xAxis, 0.0),
                vec4(yAxis, 0.0),
                vec4(zAxis, 0.0),
                vec4(vec3(0.0), 1.0));
}

void EmitBillboardVertex(vec2 xy, mat4 rotationMatrix, vec3 texCoords) {
    vec4 vertex = vec4(xy, 0.0, 0.0);
    vTexCoords = texCoords;
    vNormalMatrix = mat3(rotationMatrix);
    vCurrentPosition = vertex.xyz;
    vertex = rotationMatrix * vertex;
    vertex = vertex + gl_in[0].gl_Position;
    vertex = uCameraSpaceMat * vertex;
    gl_Position = vertex;
    EmitVertex();
}

void main() {
    vec4 probePosition = gl_in[0].gl_Position;
    vec3 texCoords = (uWorldBoudningBoxTransform * probePosition).xyz;
    mat4 rotationMatrix = RotationMatrix(probePosition.xyz);

    EmitBillboardVertex(vec2(-uRadius, -uRadius), rotationMatrix, texCoords);
    EmitBillboardVertex(vec2(-uRadius, uRadius), rotationMatrix, texCoords);
    EmitBillboardVertex(vec2(uRadius, -uRadius), rotationMatrix, texCoords);
    EmitBillboardVertex(vec2(uRadius, uRadius), rotationMatrix, texCoords);

    EndPrimitive();
}

