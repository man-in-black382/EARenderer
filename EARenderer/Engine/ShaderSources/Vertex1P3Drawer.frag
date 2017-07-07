#version 400

uniform vec4 uColor;
uniform vec4 uHighlightColor;

out vec4 oFragColor;

void main() {
    oFragColor = uColor + uHighlightColor;
}
