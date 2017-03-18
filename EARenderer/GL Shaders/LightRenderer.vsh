#version 300 es

layout (location = 0) in vec4 a_position;
layout (location = 1) in vec2 a_texCoord;

out vec2 v_texCoord;

void main()
{
    v_texCoord = a_texCoord;
    gl_Position = a_position;
}
