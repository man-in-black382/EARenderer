#version 400

uniform mat4 uProjectionMatrix;
uniform mat4 uViewMatrix;

in vec4 aPosition;

out vec3 oEyeDirection;

void main() {
    gl_Position = vec4(((gl_VertexID & 1) << 2) - 1, (gl_VertexID & 2) * 2 - 1, 1.0, 1.0);
    mat4 noTranslationViewMat = mat4(mat3(uViewMatrix));
    mat4 noTranslationProjMat = mat4(mat3(uProjectionMatrix));
    oEyeDirection = (inverse(noTranslationViewMat * noTranslationProjMat) * gl_Position).xyz;
}

