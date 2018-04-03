#version 400 core

// Layout

layout (points) in;
layout (line_strip, max_vertices = 2) out;

// Uniforms

uniform int uProbesPerGridDimensionCount;
uniform mat4 uWorldBoudningBoxTransform;
uniform mat4 uCameraSpaceMat;

uniform samplerBuffer uClusterCenters;
uniform usamplerBuffer uProjectionClusterIndices;
uniform usamplerBuffer uProbeProjectionsMetadata;

void EmitLine(vec4 from, vec4 to) {
    gl_Position = uCameraSpaceMat * from;
    EmitVertex();
    gl_Position = uCameraSpaceMat * to;
    EmitVertex();
    EndPrimitive();
}

void main() {
    vec4 worldPosition = gl_in[0].gl_Position;

    int size = uProbesPerGridDimensionCount;
    vec3 normalizedWorldPosition = (uWorldBoudningBoxTransform * worldPosition).xyz;
    ivec3 index3D = ivec3(float(size - 1) * normalizedWorldPosition);
    int index1D = size * size * index3D.z + size * index3D.y + index3D.x;
    int metadataIndex = index1D * 2; // Data in uProbeProjectionsMetadata is represented by sequence of offset-length pairs

    uint projectionGroupOffset = texelFetch(uProbeProjectionsMetadata, metadataIndex).r;
    uint projectionGroupSize = texelFetch(uProbeProjectionsMetadata, metadataIndex + 1).r;

    for (uint i = projectionGroupOffset; i < projectionGroupOffset + projectionGroupSize; ++i) {
        uint surfelClusterIndex = texelFetch(uProjectionClusterIndices, int(i)).r;
        vec3 surfelClusterCenter = texelFetch(uClusterCenters, int(surfelClusterIndex)).xyz;
        EmitLine(worldPosition, vec4(surfelClusterCenter, 1.0));
    }

}
