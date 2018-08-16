#version 400 core

// Outputs

out float oOutput;

void main() {
    // Fixed point float representation because I'm not sure whether blending on integer render
    // target will work correctly or work at all
    oOutput = 0.000000000931323; // 0.000000000931323 is 1.0 / (2 ^ 30)
}
