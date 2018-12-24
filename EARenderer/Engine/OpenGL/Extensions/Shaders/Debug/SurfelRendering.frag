#version 400 core

#include "Constants.glsl"

in vec4 vCurrentPosition;
in vec3 vAlbedo;
in vec3 vNormal;
flat in int vSurfelIndex;

uniform float uRadius;
uniform bool uUseExternalColor;
uniform vec4 uExternalColor;
uniform sampler2D uSurfelLuminances;
uniform sampler2DArray uSurfelsGBuffer;

out vec4 oFragColor;

void main() {
    // Draw a circle
    // Don't forget to exclude W component from length calculation
    if (length(vCurrentPosition.xy) > uRadius) {
        discard;
    }

    if (uUseExternalColor) {
        oFragColor = uExternalColor;
    } else {
        ivec2 luminanceMapSize = textureSize(uSurfelLuminances, 0);
        int luminanceMapWidth = luminanceMapSize.x;

        ivec2 luminanceUV = ivec2(vSurfelIndex % luminanceMapWidth,
                                  vSurfelIndex / luminanceMapWidth);

        float luminance = texelFetch(uSurfelLuminances, luminanceUV, 0).r;

        oFragColor = vec4(vAlbedo * luminance * HDRNormalizationFactor, 1.0);
    }    
}
