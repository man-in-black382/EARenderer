#include "Constants.glsl"

// Builds matrix used to orient a point or a vector around the given vector
mat4 RotationMatrix(vec3 vector) {
    vec3 worldup = vec3(0.0, 1.0, 0.0);
    vec3 zAxis = vector;

    if (abs(zAxis.x) < 0.000001 && abs(zAxis.z) < 0.000001) {
        worldup = vec3(0.0, 0.0, 1.0);
    }

    vec3 xAxis = normalize(cross(worldup, zAxis));
    vec3 yAxis = cross(zAxis, xAxis);

    return mat4(vec4(xAxis, 0.0),
                vec4(yAxis, 0.0),
                vec4(zAxis, 0.0),
                vec4(0.0, 0.0, 0.0, 1.0));
}

mat4 LookAt(vec3 eye, vec3 center, vec3 up) {
    mat4 matrix;
    vec3 x, y, z;

    // Create a new coordinate system:
    z = eye - center;
    z = normalize(z);
    y = up;
    x = cross(y, z);

    // Recompute Y = Z cross X:
    y = cross(z, x);

    // The length of the cross product is equal to the area of the parallelogram, which is < 1.0 for non-perpendicular unit-length vectors; so normalize X, Y here:
    x = normalize(x);
    y = normalize(y);

    // Put everything into the resulting 4x4 matrix:
    matrix[0][0] = x.x;
    matrix[1][0] = x.y;
    matrix[2][0] = x.z;
    matrix[3][0] = dot(-x, eye);
    matrix[0][1] = y.x;
    matrix[1][1] = y.y;
    matrix[2][1] = y.z;
    matrix[3][1] = dot(-y, eye);
    matrix[0][2] = z.x;
    matrix[1][2] = z.y;
    matrix[2][2] = z.z;
    matrix[3][2] = dot(-z, eye);
    matrix[0][3] = 0;
    matrix[1][3] = 0;
    matrix[2][3] = 0;
    matrix[3][3] = 1.0f;

    return matrix;
}

// Hammersley Points on the Hemisphere
// http://holger.dammertz.org/stuff/notes_HammersleyOnHemisphere.html
float RadicalInverse_VdC(uint bits) {
    bits = (bits << 16u) | (bits >> 16u);
    bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
    bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
    bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
    bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
    return float(bits) * 2.3283064365386963e-10; // / 0x100000000
}

// Generates a uniformly distributed and stochasticlooking sampling pattern at low computational cost
// Used for quasi-Monte Carlo integration
vec2 Hammersley2D(uint i, uint N) {
    return vec2(float(i) / float(N), RadicalInverse_VdC(i));
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