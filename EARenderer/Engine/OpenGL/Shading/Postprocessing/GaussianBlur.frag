#version 400 core

// http://rastergrid.com/blog/2010/09/efficient-gaussian-blur-with-linear-sampling/

// Uniforms

uniform vec2 uBlurDirection;
uniform sampler2D uTexture;
uniform float uKernelWeights[32];
uniform float uTextureOffsets[32];
uniform int uKernelSize;
uniform int uMipLevel;

// Inputs

in vec2 vTexCoords;

// Outputs

out vec4 oFragColor;

// Functions

void main()
{
    vec2 imageResolution = vec2(textureSize(uTexture, 0));

    float texelWeight = uKernelWeights[0];
    float texOffset = uTextureOffsets[0];
    vec2 currentOffset = vec2(texOffset) / imageResolution * uBlurDirection;

    float mipLevel = float(uMipLevel);

    oFragColor = textureLod(uTexture, (vTexCoords + currentOffset), mipLevel) * texelWeight;

    for (int i = 1; i < uKernelSize; i++) {
        texelWeight = uKernelWeights[i];
        texOffset = uTextureOffsets[i];
        currentOffset = vec2(texOffset) / imageResolution * uBlurDirection;

        oFragColor += textureLod(uTexture, (vTexCoords + currentOffset), mipLevel) * texelWeight;
        oFragColor += textureLod(uTexture, (vTexCoords - currentOffset), mipLevel) * texelWeight;
    }
}
