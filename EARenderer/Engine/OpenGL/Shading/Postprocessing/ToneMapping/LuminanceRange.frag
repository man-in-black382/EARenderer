#version 400 core

// Uniforms

uniform sampler2D uLuminance;
uniform int uLOD;

// Output

layout(location = 0) out vec2 oMinMax;

// Input

in vec2 vTexCoords;

////////////////////////////////////////////////////////////
////////////////////////// Main ////////////////////////////
////////////////////////////////////////////////////////////

void main() {
    // Multiplying by 2 to account for uLuminance being twice the size of the current render target (oMinMax)
    vec2 minMax0 = texelFetch(uLuminance, 2 * ivec2(gl_FragCoord.xy), uLOD).rg;
    vec2 minMax1 = texelFetch(uLuminance, 2 * (ivec2(gl_FragCoord.xy) + ivec2(1, 0)), uLOD).rg;
    vec2 minMax2 = texelFetch(uLuminance, 2 * (ivec2(gl_FragCoord.xy) + ivec2(1, 1)), uLOD).rg;
    vec2 minMax3 = texelFetch(uLuminance, 2 * (ivec2(gl_FragCoord.xy) + ivec2(0, 1)), uLOD).rg;

    oMinMax.x = min(min(minMax0.x, minMax1.x), min(minMax2.x, minMax3.x));
    oMinMax.y = max(max(minMax0.y, minMax1.y), max(minMax2.y, minMax3.y));
}
