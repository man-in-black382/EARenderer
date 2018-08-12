#version 400 core

// Uniforms

uniform samplerCube uCubeMapTexture;
uniform sampler2D uEquirectangularMap;
uniform bool uIsCube;

// Input

in vec3 oEyeDirection;

// Output

out vec4 oFragmentColor;

// Functions

vec3 CubeSampleCoords(vec3 sampleVector) {
    int majorAxis = 0;
    vec3 a = abs(sampleVector);

    if (a.y >= a.x && a.y >= a.z) {
        majorAxis = 1;
    } else if (a.z >= a.x && a.z >= a.y) {
        majorAxis = 2;
    }

    if (sampleVector[majorAxis] < 0.0) {
        majorAxis = majorAxis * 2 + 1;
    } else {
        majorAxis *= 2;
    }

    float tmpS = 0.0, tmpT = 0.0, m = 0.0, face = 0.0;

    switch (majorAxis) {
            /* +X */ case 0: tmpS = -sampleVector.z; tmpT = -sampleVector.y; m = a.x; face = 0.0; break;
            /* -X */ case 1: tmpS =  sampleVector.z; tmpT = -sampleVector.y; m = a.x; face = 1.0; break;
            /* +Y */ case 2: tmpS =  sampleVector.x; tmpT =  sampleVector.z; m = a.y; face = 2.0; break;
            /* -Y */ case 3: tmpS =  sampleVector.x; tmpT = -sampleVector.z; m = a.z; face = 3.0; break;
            /* +Z */ case 4: tmpS =  sampleVector.x; tmpT = -sampleVector.y; m = a.z; face = 4.0; break;
            /* -Z */ case 5: tmpS = -sampleVector.x; tmpT = -sampleVector.y; m = a.z; face = 5.0; break;
    }

    return vec3(0.5 * (tmpS / m + 1.0), 0.5 * (tmpT / m + 1.0), face);
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

//    oFragmentColor = vec4(1.0, 0.5, 1.0, 1.0);
}
