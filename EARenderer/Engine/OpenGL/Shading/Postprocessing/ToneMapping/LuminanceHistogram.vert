#version 400 core

// Uniforms

uniform sampler2D uLuminance;
uniform int uLuminanceMaxLOD;
uniform int uHistogramWidth;

// Output

// Functions

// Efficient Histogram Generation Using Scattering on GPUs
// https://developer.amd.com/wordpress/media/2012/10/GPUHistogramGeneration_preprint.pdf

void main() {
    ivec2 imageSize = ivec2(textureSize(uLuminance, 0));
    int x = gl_VertexID % imageSize.x;
    int y = gl_VertexID / imageSize.x;
    ivec2 fetchCoords = ivec2(x, y);

    // Min and max luminance are located in the highest mip of the luminance texture
    vec2 luminanceRange = texelFetch(uLuminance, ivec2(0), uLuminanceMaxLOD).rg;
    float minLuminance = luminanceRange.x;
    float maxLuminance = luminanceRange.y;

    float luminance = texelFetch(uLuminance, fetchCoords, 0).r;
    float normalizedLuminance = (luminance - minLuminance) / (maxLuminance - minLuminance);

    float bucket = normalizedLuminance * float(uHistogramWidth);
    float bucketInNDCSpace = -1.0 + ((bucket + 0.5) / 128.0);

    gl_Position = vec4(bucketInNDCSpace, 0.0, -1.0, 1.0);
}
