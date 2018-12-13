struct DirectionalLight {
    vec3 radiantFlux; // a.k.a color
    vec3 direction;
};

struct PointLight {
    vec3 radiantFlux; // a.k.a color
    vec3 position;
    float nearPlane;
    float farPlane;
};

struct Spotlight {
    vec3 radiantFlux; // a.k.a color
    vec3 position;
    vec3 direction;
    float cutOffAngleCos;
};

vec3 PointLightRadiance(PointLight light, vec3 surfaceWorldPosition) {
    float surfaceToLightDistance = distance(light.position, surfaceWorldPosition);

    // https://gamedev.stackexchange.com/a/56934
    float attenuation = clamp(1.0 - surfaceToLightDistance * surfaceToLightDistance / (light.farPlane * light.farPlane), 0.0, 1.0);
    attenuation *= attenuation;

    return light.radiantFlux * attenuation;
}

vec3 DirectionalLightRadiance(DirectionalLight light) {
    return light.radiantFlux;
}
