#version 400 core

// Uniforms

uniform samplerCube uCubeMapTexture;
uniform sampler2D uEquirectangularMap;
uniform bool uIsHDR;
uniform bool uIsCube;

// Input

in vec3 oEyeDirection;

// Output

out vec4 oFragmentColor;

// Functions

vec3 ReinhardToneMapAndGammaCorrect(vec3 color) {
    vec3 mappedColor = color / (color + vec3(1.0));
    vec3 gammaCorrectedColor = pow(mappedColor, vec3(1.0 / 2.2));
    return gammaCorrectedColor;
}

vec3 SampleSphericalMap(vec3 v) {
    const vec2 kInvAtan = vec2(0.1591, 0.3183);
    
    vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
    uv *= kInvAtan;
    uv += 0.5;
    return texture(uEquirectangularMap, uv).rgb;
}

void main() {
    if (uIsCube) {
        oFragmentColor = textureLod(uCubeMapTexture, oEyeDirection, 0);
    } else {
        oFragmentColor = vec4(SampleSphericalMap(normalize(oEyeDirection.xyz)), 1.0); // Don't forget to normalize!
    }
    
    if (uIsHDR) {
        oFragmentColor = vec4(ReinhardToneMapAndGammaCorrect(oFragmentColor.rgb), 1.0);
    }
}
