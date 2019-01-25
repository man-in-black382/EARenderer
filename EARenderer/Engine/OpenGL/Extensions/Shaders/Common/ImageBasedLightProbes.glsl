struct IBLProbe {
    samplerCube specularIrradiance;
    samplerCube diffuseIrradiance;
    sampler2D BRDFIntegrationMap;
    int specularIrradianceMipCount;
};

vec3 IBL(vec3 N, vec3 V, float roughness, vec3 fresnel, IBLProbe probe) {
    vec3 R = reflect(-V, N);

    float mip = roughness * (probe.specularIrradianceMipCount - 1);
    vec3 specularIrradiance = textureLod(probe.specularIrradiance, R, mip).rgb;

    float dotNV = max(dot(N, V), 0.0);
    vec2 envBRDF = texture(probe.BRDFIntegrationMap, vec2(dotNV, roughness)).rg;

    vec3 Ks = fresnel; // Specular (reflected) portion
    vec3 specular = (Ks * envBRDF.x + envBRDF.y) * specularIrradiance;

    return specular;
}
