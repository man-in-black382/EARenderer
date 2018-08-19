#version 400 core

// Uniforms

uniform sampler2D uHistogram;

// How much to discard from low and high ends of the histogram
// Values should be in [0; 1] range, e.g. 0.2 - discard 20%
uniform float uLowEndCutoff;
uniform float uHighEndCutoff;

// Outputs

out float oOutput;

void main() {
    float histogramWidth = float(textureSize(uHistogram, 0).x);

    int lowEndBucket = int(floor(histogramWidth * uLowEndCutoff));
    int highEndBucket = int(ceil(histogramWidth * (1.0 - uHighEndCutoff)));

    

    // Fixed point float representation because I'm not sure whether blending on integer render
    // target will work correctly or work at all
    oOutput = 0.000000000931323; // 0.000000000931323 is 1.0 / (2 ^ 30)
}
