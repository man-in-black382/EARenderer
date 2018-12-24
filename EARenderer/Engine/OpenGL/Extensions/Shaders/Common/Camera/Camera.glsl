struct Camera {
    float nearPlane;
    float farPlane;
    vec4 position;  // vec4 for alighnment purposes
    mat4 view;
    mat4 projection;
    mat4 inverseView;
    mat4 inverseProjection;
};