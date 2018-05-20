#version 400 core

// Layout

layout (points) in;
layout (line_strip, max_vertices = 512) out;

// Uniforms

uniform mat4 uCameraSpaceMat;

uniform sampler2D uProbeOcclusionMapsAtlas;

// Input

in InterfaceBlock {
    int instanceID;
} gs_in[];

// Output

// Functions

void EmitLine(vec4 from, vec4 to) {
    gl_Position = uCameraSpaceMat * from;
    EmitVertex();
    gl_Position = uCameraSpaceMat * to;
    EmitVertex();
    EndPrimitive();
}

vec3 SampleVector(int x, int y) {

    int face = gs_in[0].instanceID;
    ivec2 faceSize = ivec2(10.0);

    float s = (float(x) + 0.5) / faceSize.x * 2.0 - 1.0;
    float t = (float(y) + 0.5) / faceSize.y * 2.0 - 1.0;

    vec3 sampleVector = vec3(0.0);

    switch (face) {
        case 4: sampleVector.x =  s; sampleVector.y = -t; sampleVector.z =  1; break;
        case 5: sampleVector.x = -s; sampleVector.y = -t; sampleVector.z = -1; break;
        case 1: sampleVector.x = -1; sampleVector.y = -t; sampleVector.z =  s; break;
        case 0: sampleVector.x =  1; sampleVector.y = -t; sampleVector.z = -s; break;
        case 2: sampleVector.x =  s; sampleVector.y =  1; sampleVector.z =  t; break;
        case 3: sampleVector.x =  s; sampleVector.y = -1; sampleVector.z = -t; break;
    }

    // Normalize vector
    float invLen = 1.0 / sqrt(sampleVector.x * sampleVector.x + sampleVector.y * sampleVector.y + sampleVector.z * sampleVector.z);
    sampleVector = vec3(sampleVector.x * invLen, sampleVector.y * invLen, sampleVector.z * invLen);

    return sampleVector;
}

float OcclusionDistance(int x, int y) {
    ivec2 faceSize = ivec2(10.0);
    int face = gs_in[0].instanceID;
    int offsetX = x + face * faceSize.x;
    int offsetY = y;

    return texelFetch(uProbeOcclusionMapsAtlas, ivec2(offsetX, offsetY), 0).r;
}

void main() {
    vec4 worldPosition = gl_in[0].gl_Position;

    ivec2 faceSize = ivec2(10.0);

    for (int y = 0; y < faceSize.y; ++y) {
        for (int x = 0; x < faceSize.x; ++x) {
            vec3 sampleVector = normalize(SampleVector(x, y));
            float occlusionDistance = OcclusionDistance(x, y);

            vec4 from = worldPosition;
            vec4 to = vec4(worldPosition.xyz + sampleVector * occlusionDistance, 1.0);

            EmitLine(from, to);
        }
    }

}
