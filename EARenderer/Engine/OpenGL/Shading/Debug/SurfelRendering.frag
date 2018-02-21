#version 400 core

in vec4 iCurrentPosition;

uniform float uRadius;

out vec4 oFragColor;

void main() {
    // Draw a circle
    // Don't forget to exclude W component from length calculation
    if (length(iCurrentPosition.xy) > uRadius) {
        discard;
    }

    oFragColor = vec4(0.5, 1.0, 0.8, 1.0);
}
