#version 400 core

const vec2 kVertices[4] = vec2[]
(
 vec2(-1.0,  1.0),
 vec2(-1.0, -1.0),
 vec2( 1.0,  1.0),
 vec2( 1.0, -1.0)
 );

void main() {
    gl_Position = vec4(kVertices[gl_VertexID], 1.0, 1.0);
}


