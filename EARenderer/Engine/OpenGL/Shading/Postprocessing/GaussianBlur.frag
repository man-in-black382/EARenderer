#version 400 core

// http://rastergrid.com/blog/2010/09/efficient-gaussian-blur-with-linear-sampling/

// Uniforms

uniform vec2 uBlurDirection;
uniform sampler2D uTexture;
uniform vec2 uKernelData[32];
uniform int uKernelSize;

// Inputs

in vec2 vTexCoords;

// Outputs

out vec4 oFragColor;

// Functions

void main()
{
    vec2 imageResolution = vec2(textureSize(uTexture, 0));

    float texelWeight = uKernelData[0].x;
    float texOffset = uKernelData[0].y;
    vec2 currentOffset = vec2(texOffset) / imageResolution * uBlurDirection;

    oFragColor = texture2D(uTexture, (vTexCoords + currentOffset) * texelWeight;

    for (int i = 1; i < uKernelSize; i++) {
        texelWeight = uKernelData[i].x;
        texOffset = uKernelData[i].y;
        currentOffset = vec2(texOffset) / imageResolution * uBlurDirection;

        oFragColor += texture2D(uTexture, (vTexCoords + currentOffset) * texelWeight;
        oFragColor += texture2D(uTexture, (vTexCoords - currentOffset) * texelWeight;
    }
}
