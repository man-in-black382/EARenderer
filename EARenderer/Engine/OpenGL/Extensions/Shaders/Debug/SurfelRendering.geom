#version 400 core

// Layout

layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

// Input

in vec3 gNormal[];
in vec3 gAlbedo[];
in float gArea[];
flat in int gSurfelIndex[];

// Uniforms

uniform mat4 uViewProjectionMatrix;
uniform float uRadius;

// Outputs
out vec4 vCurrentPosition;
out vec3 vAlbedo;
out vec3 vNormal;
flat out int vSurfelIndex;

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

void EmitBillboardVertex(vec2 cornerPoint, mat4 rotationMatrix) {
    vec4 surfelPosition = gl_in[0].gl_Position;
    vec4 localCornerPoint = vec4(cornerPoint, 0.0, 0.0);

    vCurrentPosition = localCornerPoint;
    vAlbedo = gAlbedo[0];
    vSurfelIndex = gSurfelIndex[0];
    vNormal = gNormal[0];

    localCornerPoint = rotationMatrix * localCornerPoint;
    localCornerPoint = localCornerPoint + surfelPosition + vec4(vec3(vNormal * 0.02), 0.0);
    localCornerPoint = uViewProjectionMatrix * localCornerPoint;

    gl_Position = localCornerPoint;
    EmitVertex();
}

void main() {
    mat4 rotationMatrix = RotationMatrix();

    EmitBillboardVertex(vec2(-uRadius, -uRadius), rotationMatrix);
    EmitBillboardVertex(vec2(-uRadius, uRadius), rotationMatrix);
    EmitBillboardVertex(vec2(uRadius, -uRadius), rotationMatrix);
    EmitBillboardVertex(vec2(uRadius, uRadius), rotationMatrix);

    EndPrimitive();
}

