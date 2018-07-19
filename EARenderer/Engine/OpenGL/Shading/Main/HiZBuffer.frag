#version 400 core

// Uniforms

uniform sampler2D uLinearDepthTexture;
uniform int uLOD;

// Output

layout(location = 0) out float oHiZ;

// Input

in vec2 vTexCoords;

////////////////////////////////////////////////////////////
////////////////////////// Main ////////////////////////////
////////////////////////////////////////////////////////////

void main() {
    // Multiplying by 2 to account for uLinearDepthTexture being twice the size of the current render target (oHiZ)
    float depth0 = texelFetch(uLinearDepthTexture, 2 * ivec2(gl_FragCoord.xy), uLOD).r;
    float depth1 = texelFetch(uLinearDepthTexture, 2 * (ivec2(gl_FragCoord.xy) + ivec2(1, 0)), uLOD).r;
    float depth2 = texelFetch(uLinearDepthTexture, 2 * (ivec2(gl_FragCoord.xy) + ivec2(1, 1)), uLOD).r;
    float depth3 = texelFetch(uLinearDepthTexture, 2 * (ivec2(gl_FragCoord.xy) + ivec2(0, 1)), uLOD).r;

    oHiZ = min(min(depth0, depth1), min(depth2, depth3));
}
