#version 400 core

in vec4 vCurrentPosition;
in vec3 vAlbedo;

uniform float uRadius;

out vec4 oFragColor;

vec3 GammaCorrect(vec3 color) {
    return pow(color, vec3(1.0 / 2.2));
}

void main() {
    // Draw a circle
    // Don't forget to exclude W component from length calculation
    if (length(vCurrentPosition.xy) > uRadius) {
        discard;
    }

    oFragColor = vec4(GammaCorrect(vAlbedo), 1.0);
}
