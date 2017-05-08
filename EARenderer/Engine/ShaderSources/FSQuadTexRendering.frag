#version 400

uniform sampler2D uTexture;

in vec2 oTexCoords;

out vec4 fragmentColor;

void main()
{
    fragmentColor = texture(uTexture, oTexCoords);
}
