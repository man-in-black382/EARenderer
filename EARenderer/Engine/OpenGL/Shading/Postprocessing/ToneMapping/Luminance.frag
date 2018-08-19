#version 400 core

// Uniforms

uniform sampler2D uImage;

// Input

in vec2 vTexCoords;

// Outputs

out vec2 oLuminance;

void main() {
    vec3 color = textureLod(uImage, vTexCoords, 0).rgb;
    vec3 factors = vec3(0.2126, 0.7152, 0.0722);

    // Two output channels are needed for Min/Max
    // luminance computation later on in the pipeline
    oLuminance = vec2(dot(color, factors));
}
