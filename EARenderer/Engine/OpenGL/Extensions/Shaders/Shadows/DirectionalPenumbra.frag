#version 400 core

#include "GBuffer.glsl"
#include "Constants.glsl"
#include "DirectionalShadows.glsl"

// Uniforms

uniform sampler2D uGBufferHiZBuffer;

uniform mat4 uCSMSplitSpaceMat;
uniform mat4 uLightSpaceMatrices[MaximumShadowCascadesCount];
uniform int uDepthSplitsAxis;
uniform float uDepthSplits[MaximumShadowCascadesCount];

uniform sampler2DArray uDirectionalShadowMapsBilinearSampler;

uniform mat4 uCameraViewInverse;
uniform mat4 uCameraProjectionInverse;

uniform DirectionalLight uDirectionalLight;

// Output

layout(location = 0) out float oOutput;

// Input

in vec2 vTexCoords;

void main() {
    vec3 worldPosition = ReconstructWorldPosition(uGBufferHiZBuffer, vTexCoords, uCameraViewInverse, uCameraProjectionInverse);
    int cascade = ShadowCascadeIndex(worldPosition, uCSMSplitSpaceMat, uDepthSplitsAxis, uDepthSplits);
    oOutput = DirectionalPenumbra(worldPosition, cascade, uLightSpaceMatrices, uDirectionalLight, uDirectionalShadowMapsBilinearSampler);
}
