#version 400 core

// Constants

const int kMaxCascades = 4;

// Uniforms

uniform float uESMFactor;
uniform float uDepthSplits[kMaxCascades];

// Input

in vec3 vFragClipSpacePosition;
in vec4 vPosInCameraSpace;

// Output

out vec4 oFragColor;

// Functions

int CascadeIndex()
{
    vec3 projCoords = vPosInCameraSpace.xyz / vPosInCameraSpace.w;
    // No need to transform to [0,1] range,
    // because splits passed from client are in [-1; 1]

    float fragDepth = projCoords.z;

    for (int i = 0; i < kMaxCascades; ++i) {
        if (fragDepth < uDepthSplits[i]) {
            return i;
        }
    }

    return 0;
}

void main() {
    float linearDepth = vFragClipSpacePosition.z;
    float exponent = exp(uESMFactor * linearDepth);
    int cascadeIndex = CascadeIndex();
    oFragColor[cascadeIndex] = 1.0;// exponent;

    switch (cascadeIndex) {
        case 0: oFragColor = vec4(1.0, 0.0, 0.0, 1.0); break;
        case 1: oFragColor = vec4(0.0, 1.0, 0.0, 1.0); break;
        case 2: oFragColor = vec4(0.0, 0.0, 1.0, 1.0); break;
        case 3: oFragColor = vec4(0.0, 1.0, 1.0, 1.0); break;
    }
}
