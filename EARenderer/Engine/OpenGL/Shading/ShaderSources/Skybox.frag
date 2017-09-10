#version 400 core

uniform samplerCube uCubeMapTexture;

in vec3 oEyeDirection;

out vec4 fragmentColor;

void main()
{
    fragmentColor = texture(uCubeMapTexture, oEyeDirection);
}
