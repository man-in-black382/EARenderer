#version 400 core

// Uniforms

uniform sampler2D uBaseImageTexture;
uniform sampler2D uBlurTexture;

uniform float uLargeBlurWeight;
uniform float uMediumBlurWeight;
uniform float uSmallBlurWeight;

// Inputs

in vec2 vTexCoords;

// Outputs

layout(location = 0) out vec4 oFragColor;
layout(location = 1) out float oLuminanceOutput;

// Functions

void main() {
    vec3 baseTexData       = textureLod(uBaseImageTexture,  vTexCoords, 0).rgb;
    vec3 smallBlurTexData  = textureLod(uBlurTexture, vTexCoords, 0).rgb;
    vec3 mediumBlurTexData = textureLod(uBlurTexture, vTexCoords, 1).rgb;
    vec3 largeBlurTexData  = textureLod(uBlurTexture, vTexCoords, 2).rgb;

    vec3 combined = largeBlurTexData * uLargeBlurWeight +
                    mediumBlurTexData * uMediumBlurWeight +
                    smallBlurTexData * uSmallBlurWeight;

    oFragColor = vec4(combined + baseTexData, 1.0);

    vec3 factors = vec3(0.2126, 0.7152, 0.0722);
    float luminocity = dot(oFragColor, factors);
    oLuminanceOutput = luminocity;
}
