#version 400 core

// Uniforms

uniform sampler2D uImage;
uniform int uHistogramWidth;

// Output

// Functions

// Efficient Histogram Generation Using Scattering on GPUs
// https://developer.amd.com/wordpress/media/2012/10/GPUHistogramGeneration_preprint.pdf

void main() {
    ivec2 imageSize = textureSize(uImage, 0);
    int x = gl_VertexID % imageSize.x;
    int y = gl_VertexID / imageSize.x;
    ivec2 fetchCoords = ivec2(x, y);

    vec3 color = texelFetch(uImage, fetchCoords, 0).rgb;
    vec3 factors = vec3(0.2126, 0.7152, 0.0722);
    float luminocity = dot(color, factors);

    float bucket = luminocity * float(uHistogramWidth);
    float bucketInNDCSpace = -1.0 + ((bucket + 0.5) / 128.0);

    gl_Position = vec4(bucketInNDCSpace, 0.0, -1.0, 1.0);
}
