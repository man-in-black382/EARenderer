#version 400 core

// Uniforms

uniform sampler2D uImage;

// Inputs

in vec2 vTexCoords;

// Outputs

out vec4 oFragColor;

// Functions

vec3 ReinhardToneMap(vec3 color) {
    return color / (color + vec3(1.0));
}

vec3 GammaCorrect(vec3 color) {
    return pow(color, vec3(1.0 / 2.2));
}

void main() {
    vec3 color = texture(uImage, vTexCoords).rgb;
    color = ReinhardToneMap(color);
    color = GammaCorrect(color);
    oFragColor = vec4(color, 1.0);
}
