#version 400 core

#include "GBuffer.glsl"
#include "Constants.glsl"
#include "OmnidirectionalShadows.glsl"
#include "Lights.glsl"

// Uniforms

uniform PointLight uPointLight;

uniform sampler2D uGBufferHiZBuffer;
uniform samplerCube uOmnidirectionalShadowMapBilinearSampler;

uniform mat4 uCameraViewInverse;
uniform mat4 uCameraProjectionInverse;

// Output

layout(location = 0) out float oOutput;

// Input

in vec2 vTexCoords;

void main() {
    vec3 worldPosition = ReconstructWorldPosition(uGBufferHiZBuffer, vTexCoords, uCameraViewInverse, uCameraProjectionInverse);
    oOutput = OmnidirectionalPenumbra(worldPosition, uPointLight, uOmnidirectionalShadowMapBilinearSampler);
}
