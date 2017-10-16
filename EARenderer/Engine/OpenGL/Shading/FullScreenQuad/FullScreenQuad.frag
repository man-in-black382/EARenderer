#version 400 core

//uniform sampler2D uTexture;
uniform sampler2DArray uTextureArray;
uniform int uIndex;

in vec2 vTexCoords;

out vec4 fragmentColor;

void main()
{
//    fragmentColor = texture(uTexture, oTexCoords);
    fragmentColor = vec4(vec3(texture(uTextureArray, vec3(vTexCoords, uIndex)).r), 1.0);
}
