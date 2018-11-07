struct DirectionalLight {
    vec3 radiantFlux; // a.k.a color
    vec3 direction;
};

struct PointLight {
    vec3 radiantFlux; // a.k.a color
    vec3 position;
    float clipDistance;
};

struct Spotlight {
    vec3 radiantFlux; // a.k.a color
    vec3 position;
    vec3 direction;
    float cutOffAngleCos;
};

vec3 PointLightRadiance(PointLight light, vec3 surfaceWorldPosition) {
    vec3 Wi                 = normalize(light.position - surfaceWorldPosition);   // To light vector
    float distance          = length(Wi);                                         // Distance from fragment to light
    float attenuation       = 1.0 / (distance * distance);                        // How much enegry has light lost at current distance

    return light.radiantFlux * attenuation;
}

vec3 DirectionalLightRadiance(DirectionalLight light) {
    return light.radiantFlux;
}
