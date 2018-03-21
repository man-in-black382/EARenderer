#version 400 core

uniform sampler2D uTexture;
uniform sampler2DArray uTextureArray;
uniform int uIndex;
uniform bool uShouldSampleArray;

in vec2 vTexCoords;

out vec4 oFragColor;

void main()
{
    if (uShouldSampleArray) {
        oFragColor = vec4(vec3(texture(uTextureArray, vec3(vTexCoords, uIndex))), 1.0);
    } else {
        oFragColor = texture(uTexture, vTexCoords);
    }
}
