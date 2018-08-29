#version 410 core

// Output

out InterfaceBlock {
    vec2 texCoords;
    int instanceID;
} vs_out;

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
    vs_out.texCoords.xy = gl_Position.xy / 2.0 + 0.5;
    vs_out.instanceID = gl_InstanceID;
}

