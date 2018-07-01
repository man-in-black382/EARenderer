#version 400 core

// Uniforms

uniform sampler2D uBaseImageTexture;
uniform sampler2D uLargeBlurTexture;
uniform sampler2D uMediumBlurTexture;
uniform sampler2D uSmallBlurTexture;

uniform float uLargeBlurWeight;
uniform float uMediumBlurWeight;
uniform float uSmallBlurWeight;

// Inputs

in vec2 vTexCoords;

// Outputs

out vec4 oFragColor;

// Functions

void main() {
    vec3 baseTexData       = texture(uBaseImageTexture,  vTexCoords).rgb;
    vec3 largeBlurTexData  = texture(uLargeBlurTexture,  vTexCoords).rgb;
    vec3 mediumBlurTexData = texture(uMediumBlurTexture, vTexCoords).rgb;
    vec3 smallBlurTexData  = texture(uSmallBlurTexture,  vTexCoords).rgb;

    vec3 combined = largeBlurTexData * uLargeBlurWeight +
                    mediumBlurTexData * uMediumBlurWeight +
                    smallBlurTexData * uSmallBlurWeight;

    oFragColor = vec4(combined + baseTexData, 1.0);
}
