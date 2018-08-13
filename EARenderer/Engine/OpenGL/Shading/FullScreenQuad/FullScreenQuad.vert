#version 400 core

out vec2 vTexCoords;

const vec2 kVertices[4] = vec2[]
(
 vec2(-1.0,  1.0),
 vec2(-1.0, -1.0),
 vec2( 1.0,  1.0),
 vec2( 1.0, -1.0)
 );

void main() {
    gl_Position = vec4(kVertices[gl_VertexID], -0.99, 1.0);
    vTexCoords = gl_Position.xy / 2.0 + 0.5;
}

