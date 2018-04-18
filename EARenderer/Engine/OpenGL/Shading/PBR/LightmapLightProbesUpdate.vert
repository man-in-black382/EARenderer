#version 400 core

// Output

out vec2 vTexCoords;

// Functions

const vec2 kVertices[4] = vec2[]
(
 vec2(-1.0,  1.0),
 vec2(-1.0, -1.0),
 vec2( 1.0,  1.0),
 vec2( 1.0, -1.0)
 );

void main() {
    gl_Position = vec4(kVertices[gl_VertexID], 1.0, 1.0);
    vTexCoords.xy = gl_Position.xy / 2.0 + 0.5;
}

