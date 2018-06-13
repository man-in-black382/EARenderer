#version 400 core

// https://github.com/mattdesl/lwjgl-basics/wiki/shaderlesson5

// Uniforms

uniform vec2 uBlurDirection;
uniform vec2 uFBOResolution;
uniform float uBlurRadius;

uniform sampler2D uTexture;

// Inputs

in vec2 vTexCoords;

// Outputs

out vec4 oFragColor;

// Functions

void main()
{
    // Doing large kernels per pixel might be worse than doing multi-pass with smaller localized kernels access because of texture cache (and large kernels are wasteful and inefficient).
    // https://software.intel.com/en-us/blogs/2014/07/15/an-investigation-of-fast-real-time-gpu-based-image-blur-algorithms


    // This will be our RGBA sum
    vec4 sum = vec4(0.0);

    // Our original texcoord for this fragment
    vec2 tc = vTexCoord;

    // The amount to blur, i.e. how far off center to sample from
    // 1.0 -> blur by one pixel
    // 2.0 -> blur by two pixels, etc.
    float blur = uBlurRadius / uFBOResolution;

    // The direction of our blur
    // (1.0, 0.0) -> x-axis blur
    // (0.0, 1.0) -> y-axis blur
    float hstep = uBlurDirection.x;
    float vstep = uBlurDirection.y;

    // Apply blurring, using a 9-tap filter with predefined gaussian weights

    sum += texture2D(uTexture, vec2(tc.x - 4.0*blur*hstep, tc.y - 4.0*blur*vstep)) * 0.0162162162;
    sum += texture2D(uTexture, vec2(tc.x - 3.0*blur*hstep, tc.y - 3.0*blur*vstep)) * 0.0540540541;
    sum += texture2D(uTexture, vec2(tc.x - 2.0*blur*hstep, tc.y - 2.0*blur*vstep)) * 0.1216216216;
    sum += texture2D(uTexture, vec2(tc.x - 1.0*blur*hstep, tc.y - 1.0*blur*vstep)) * 0.1945945946;

    sum += texture2D(uTexture, vec2(tc.x, tc.y)) * 0.2270270270;

    sum += texture2D(uTexture, vec2(tc.x + 1.0*blur*hstep, tc.y + 1.0*blur*vstep)) * 0.1945945946;
    sum += texture2D(uTexture, vec2(tc.x + 2.0*blur*hstep, tc.y + 2.0*blur*vstep)) * 0.1216216216;
    sum += texture2D(uTexture, vec2(tc.x + 3.0*blur*hstep, tc.y + 3.0*blur*vstep)) * 0.0540540541;
    sum += texture2D(uTexture, vec2(tc.x + 4.0*blur*hstep, tc.y + 4.0*blur*vstep)) * 0.0162162162;

    // Discard alpha for our simple demo, multiply by vertex color and return
    oFragColor = vColor * vec4(sum.rgb, 1.0);
}
