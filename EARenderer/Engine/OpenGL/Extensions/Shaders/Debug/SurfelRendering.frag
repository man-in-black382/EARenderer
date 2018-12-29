#version 400 core

#include "Constants.glsl"

in vec4 vCurrentPosition;
in vec3 vAlbedo;
in vec3 vNormal;
flat in int vSurfelIndex;

uniform float uRadius;
uniform bool uUseExternalColor;
uniform vec3 uExternalColor;
uniform sampler2D uSurfelLuminances;
uniform sampler2DArray uSurfelsGBuffer;

out vec4 oFragColor;

void main() {
    // Draw a circle
    // Don't forget to exclude W component from length calculation
    if (length(vCurrentPosition.xy) > uRadius) {
        discard;
    }

    ivec2 luminanceMapSize = textureSize(uSurfelLuminances, 0);
    int luminanceMapWidth = luminanceMapSize.x;

    ivec2 luminanceUV = ivec2(vSurfelIndex % luminanceMapWidth,
                              vSurfelIndex / luminanceMapWidth);

    float luminance = texelFetch(uSurfelLuminances, luminanceUV, 0).r;

    vec3 color = uUseExternalColor ? uExternalColor : vAlbedo;
    oFragColor = vec4(color * luminance * HDRNormalizationFactor, 1.0);
}
