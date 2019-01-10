vec3 YCoCg_From_RGB(vec3 rgb) {
    float Co = (rgb.r - rgb.b) / 2.0;
    float t = rgb.b + Co;
    float Cg = (rgb.g - t) / 2.0;
    float Y = t + Cg;
    return vec3(Y, Co, Cg);
}

vec3 RGB_From_YCoCg(vec3 YCoCg) {
    float t = YCoCg.x - YCoCg.z;
    float g = YCoCg.x + YCoCg.z;
    float b = t - YCoCg.y;
    float r = t + YCoCg.y;
    return vec3(r, g, b);
}

float LuminanceFromRGB(vec3 rgb) {
    vec3 factors = vec3(0.2126, 0.7152, 0.0722);
    return dot(rgb, factors);
}

vec3 SRGB_From_RGB(vec3 rgb) {
    return pow(rgb, vec3(1.0 / 2.2));
}
