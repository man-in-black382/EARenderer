#include "Constants.glsl"

// Builds matrix used to orient a point or a vector around the given vector
mat4 RotationMatrix(vec3 vector) {
    vec3 worldUp = vec3(0.0, 1.0, 0.0);
    vec3 zAxis = vector;

    if (abs(zAxis.x) < 0.000001 && abs(zAxis.z) < 0.000001) {
        worldUp = vec3(0.0, 0.0, 1.0);
    }

    vec3 xAxis = normalize(cross(worldUp, zAxis));
    vec3 yAxis = cross(zAxis, xAxis);

    return mat4(vec4(xAxis, 0.0),
                vec4(yAxis, 0.0),
                vec4(zAxis, 0.0),
                vec4(0.0, 0.0, 0.0, 1.0));
}

// http://holger.dammertz.org/stuff/notes_HammersleyOnHemisphere.html
vec3 HemisphereSampleUniform(float u, float v) {
     float phi = v * 2.0 * PI;
     float cosTheta = 1.0 - u;
     float sinTheta = sqrt(1.0 - cosTheta * cosTheta);
     return vec3(cos(phi) * sinTheta, sin(phi) * sinTheta, cosTheta);
 }

 vec3 HemisphereSampleCosine(float u, float v) {
     float phi = v * 2.0 * PI;
     float cosTheta = sqrt(1.0 - u);
     float sinTheta = sqrt(1.0 - cosTheta * cosTheta);
     return vec3(cos(phi) * sinTheta, sin(phi) * sinTheta, cosTheta);
 }

 float LinearDepth(float hyperbolicDepth, float nearPlane, float farPlane) {
     float depth = hyperbolicDepth;// * 2.0 - 1.0;
     return nearPlane * farPlane / (farPlane + depth * (nearPlane - farPlane));
 }

 bool RayPlaneIntersection(vec3 pointOnPlane, vec3 planeNormal, vec3 rayOrigin, vec3 rayDirection, inout float t)  {
     // Assuming vectors are all normalized
     float denom = dot(planeNormal, rayDirection);
     vec3 p0l0 = pointOnPlane - rayOrigin;
     t = dot(p0l0, planeNormal) / denom;
     return (t >= 0);
 }