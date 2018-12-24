struct DirectionalLight {
    vec3 radiantFlux; // a.k.a color
    vec3 direction;
    float area;
};

struct PointLight {
    vec3 radiantFlux; // a.k.a color
    vec3 position;
    float nearPlane;
    float farPlane;
    float area;
    // Attenuation factors
    float constant;
    float linear;
    float quadratic;
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
    float attenuation = 1.0 / (
        light.constant +
        light.linear * surfaceToLightDistance +
        light.quadratic * (surfaceToLightDistance * surfaceToLightDistance)
    );

    return light.radiantFlux * attenuation;
}

vec3 DirectionalLightRadiance(DirectionalLight light) {
    return light.radiantFlux;
}
