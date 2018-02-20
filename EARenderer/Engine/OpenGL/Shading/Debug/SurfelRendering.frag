#version 400 core

in vec4 iCenter;
in vec4 iCurrent;

uniform float uRadius;

out vec4 oFragColor;

void main() {
    // Draw a circle
    if (length(iCurrent - iCenter) > uRadius) {
        discard;
    }

    oFragColor = vec4(0.2, 1.0, 0.4, 1.0);
}
