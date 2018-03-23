#version 400 core

uniform sampler2D uTexture;
uniform sampler2DArray uTextureArray;
uniform int uIndex;
uniform bool uShouldSampleArray;
uniform bool uShouldApplyToneMapping;

in vec2 vTexCoords;

out vec4 oFragColor;

vec3 ReinhardToneMap(vec3 color) {
    return color / (color + vec3(1.0));
}

void main()
{
    if (uShouldSampleArray) {
        vec3 coords = vec3(vTexCoords, uIndex);
        oFragColor = vec4(texture(uTextureArray, coords).rgb, 1.0);
    } else {
        oFragColor = vec4(texture(uTexture, vTexCoords).rgb, 1.0);
    }

    if (uShouldApplyToneMapping) {
        oFragColor = vec4(ReinhardToneMap(oFragColor.rgb), oFragColor.a);
    }
}
