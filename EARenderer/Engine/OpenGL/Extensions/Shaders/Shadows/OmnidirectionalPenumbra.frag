#version 400 core

#include "GBuffer.glsl"
#include "Constants.glsl"
#include "OmnidirectionalShadows.glsl"
#include "PointLightUBO.glsl"

// Uniforms

uniform usampler2D uMaterialData;
uniform sampler2D uGBufferHiZBuffer;
uniform samplerCube uOmnidirectionalShadowMapBilinearSampler;

uniform mat4 uCameraViewInverse;
uniform mat4 uCameraProjectionInverse;

// Output

layout(location = 0) out float oOutput;

// Input

in vec2 vTexCoords;

void main() {
    uvec4 materialData = texture(uMaterialData, vTexCoords);
    vec3 normal = DecodeGBufferCookTorranceNormal(materialData);
    vec3 worldPosition = ReconstructWorldPosition(uGBufferHiZBuffer, vTexCoords, uCameraViewInverse, uCameraProjectionInverse);
    oOutput = OmnidirectionalPenumbra(worldPosition, normal, uboPointLight, uOmnidirectionalShadowMapBilinearSampler);
}
