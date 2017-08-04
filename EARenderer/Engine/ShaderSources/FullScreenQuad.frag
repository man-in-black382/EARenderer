#version 400 core

uniform sampler2D uTexture;

in vec2 oTexCoords;

out vec4 fragmentColor;

void main()
{
    fragmentColor = vec4(vec3(texture(uTexture, oTexCoords).r), 1.0);
}
