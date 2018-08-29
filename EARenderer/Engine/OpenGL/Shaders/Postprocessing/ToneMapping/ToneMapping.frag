#version 400 core

// Uniforms

uniform sampler2D uImage;
uniform sampler2D uExposure;

// Inputs

in vec2 vTexCoords;

// Outputs

out vec4 oFragColor;

// Constants

// Should be in range [0.0; 1.0]
const float ShoulderStrength = 0.22;

// Should be in range [0.0; 1.0]
const float LinearStrength = 0.30;

// Should be in range [0.0; 1.0]
const float LinearAngle = 0.10;

// Should be in range [0.0; 1.0]
const float ToeStrength = 0.20;

// Should be in range [0.0; 1.0]
const float ToeNumerator = 0.20;

// Should be in range [0.0; 1.0]
const float ToeDenominator = 0.22;

// Should be in range [0.0; 20.0]
const float LinearWhitePoint = 11.2;

const float ExposureBias = 2.0;

const float ExposureNominator = 16.0;

// Functions

// https://github.com/Zackin5/Filmic-Tonemapping-ReShade/blob/master/Uncharted2.fx

vec3 Uncharted2Tonemap(vec3 x)
{
    return ((x * (ShoulderStrength * x + LinearAngle * LinearStrength) + ToeStrength * ToeNumerator)
            / (x * (ShoulderStrength * x + LinearStrength) + ToeStrength * ToeDenominator)) - ToeNumerator / ToeDenominator;
}

vec3 ReinhardToneMap(vec3 color) {
    return color / (color + vec3(1.0));
}

vec3 GammaCorrect(vec3 color) {
    return pow(color, vec3(1.0 / 2.2));
}

void main() {
    float exposure = texelFetch(uExposure, ivec2(0), 0).r;

    vec3 color = textureLod(uImage, vTexCoords, 0).rgb;
    color *= 0.001;//exposure;  // Exposure Adjustment

    bool tonemapOnLuminance = true;

    // Do tonemapping on RGB or Luminance
    if(!tonemapOnLuminance) {
        color = Uncharted2Tonemap(ExposureBias * color);
    } else {
        float lum = 0.2126 * color.r + 0.7152 * color.g + 0.0722 * color.b;
        vec3 newLum = Uncharted2Tonemap(vec3(ExposureBias * lum));
        vec3 lumScale = newLum / lum;
        color *= lumScale;
    }

    vec3 whiteScale = 1.0f / Uncharted2Tonemap(vec3(LinearWhitePoint));
    color *= whiteScale;

    color = GammaCorrect(color);

    oFragColor = vec4(color, 1.0);
}
