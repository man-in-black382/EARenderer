struct CookTorranceMaterialOverrides {
    vec4 albedo; // vec4 for alighnment purposes
    vec4 normal; // vec4 for alighnment purposes
    float metalness;
    float roughness;
    float AO;
    float displacement;

    bool isAlbedoOverriden;
    bool isNormalOverriden;
    bool isMetalnessOverriden;
    bool isRoughnessOverriden;
    bool isAOOverriden;
    bool isDisplacementOverriden;
};