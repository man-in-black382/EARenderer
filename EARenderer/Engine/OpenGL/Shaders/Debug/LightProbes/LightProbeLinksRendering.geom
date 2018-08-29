#version 400 core

// Layout

layout (points) in;
layout (line_strip, max_vertices = 512) out;

// Uniforms

uniform ivec3 uProbesGridResolution;
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

// Transform normalized texture corrdinates into a 3-dimensional integer index
int FlattenTexCoords() {
    vec4 worldPosition = gl_in[0].gl_Position;
    vec3 normalizedWorldPosition = (uWorldBoudningBoxTransform * worldPosition).xyz;

    // Texture coordinate interpolation gives us values at texel centers, not edges
    // and so we're accounting for that by adding half a texel size to x and y
    vec3 halfTexel = 1.0 / vec3(uProbesGridResolution) / 2.0;
    vec3 resolution = vec3(uProbesGridResolution - 1);
    float x = (normalizedWorldPosition.x + halfTexel.x) * resolution.x;
    float y = (normalizedWorldPosition.y + halfTexel.y) * resolution.y;
    float z = (normalizedWorldPosition.z + halfTexel.z) * resolution.z;

    int index = uProbesGridResolution.y * uProbesGridResolution.x * int(z) + uProbesGridResolution.x * int(y) + int(x);
    return index;
}

void main() {
    vec4 worldPosition = gl_in[0].gl_Position;
    
    int metadataIndex = FlattenTexCoords() * 2; // Data in uProbeProjectionsMetadata is represented by sequence of offset-length pairs

    uint projectionGroupOffset = texelFetch(uProbeProjectionsMetadata, metadataIndex).r;
    uint projectionGroupSize = texelFetch(uProbeProjectionsMetadata, metadataIndex + 1).r;

    for (uint i = projectionGroupOffset; i < projectionGroupOffset + projectionGroupSize; ++i) {
        uint surfelClusterIndex = texelFetch(uProjectionClusterIndices, int(i)).r;
        vec3 surfelClusterCenter = texelFetch(uClusterCenters, int(surfelClusterIndex)).xyz;
        EmitLine(worldPosition, vec4(surfelClusterCenter, 1.0));
    }
}
