const float Y00 = 0.28209479177387814347; // 1 / (2*sqrt(pi))
const float Y11 = -0.48860251190291992159; // sqrt(3 /(4pi))
const float Y10 = 0.48860251190291992159;
const float Y1_1 = -0.48860251190291992159;
const float Y21 = -1.09254843059207907054; // 1 / (2*sqrt(pi))
const float Y2_1 = -1.09254843059207907054;
const float Y2_2 = 1.09254843059207907054;
const float Y20 = 0.31539156525252000603; // 1/4 * sqrt(5/pi)
const float Y22 = 0.54627421529603953527; // 1/4 * sqrt(15/pi)

struct SH {
    vec3 L00;
    vec3 L11;
    vec3 L10;
    vec3 L1_1;
    vec3 L21;
    vec3 L2_1;
    vec3 L2_2;
    vec3 L20;
    vec3 L22;
};

SH ZeroSH() {
    SH result;

    result.L00  = vec3(0.0);

    result.L1_1 = vec3(0.0);
    result.L10  = vec3(0.0);
    result.L11  = vec3(0.0);

    result.L2_2 = vec3(0.0);
    result.L2_1 = vec3(0.0);
    result.L21  = vec3(0.0);

    result.L20  = vec3(0.0);

    result.L22  = vec3(0.0);

    return result;
}

SH ScaleSH(SH sh, vec3 color) {
    SH result;

    result.L00  = color * sh.L00;

    result.L1_1 = color * sh.L1_1;
    result.L10  = color * sh.L10;
    result.L11  = color * sh.L11;

    result.L2_2 = color * sh.L2_2;
    result.L2_1 = color * sh.L2_1;
    result.L21  = color * sh.L21;

    result.L20  = color * sh.L20;

    result.L22  = color * sh.L22;

    return result;
}

SH Sum2SH(SH first, SH second) {
    SH result;

    result.L00  = first.L00  + second.L00;

    result.L1_1 = first.L1_1 + second.L1_1;
    result.L10  = first.L10  + second.L10;
    result.L11  = first.L11  + second.L11;

    result.L2_2 = first.L2_2 + second.L2_2;
    result.L2_1 = first.L2_1 + second.L2_1;
    result.L21  = first.L21  + second.L21;

    result.L20  = first.L20  + second.L20;

    result.L22  = first.L22  + second.L22;

    return result;
}

SH Sum8SH(SH first, SH second, SH third, SH fourth, SH fifth, SH sixth, SH seventh, SH eighth) {
    SH result;

    result.L00  = first.L00 + second.L00 + third.L00 + fourth.L00 + fifth.L00 + sixth.L00 + seventh.L00 + eighth.L00;

    result.L1_1 = first.L1_1 + second.L1_1 + third.L1_1 + fourth.L1_1 + fifth.L1_1 + sixth.L1_1 + seventh.L1_1 + eighth.L1_1;

    result.L10  = first.L10 + second.L10 + third.L10 + fourth.L10 + fifth.L10 + sixth.L10 + seventh.L10 + eighth.L10;

    result.L11  = first.L11 + second.L11 + third.L11 + fourth.L11 + fifth.L11 + sixth.L11 + seventh.L11 + eighth.L11;

    result.L2_2 = first.L2_2 + second.L2_2 + third.L2_2 + fourth.L2_2 + fifth.L2_2 + sixth.L2_2 + seventh.L2_2 + eighth.L2_2;

    result.L2_1 = first.L2_1 + second.L2_1 + third.L2_1 + fourth.L2_1 + fifth.L2_1 + sixth.L2_1 + seventh.L2_1 + eighth.L2_1;

    result.L21  = first.L21 + second.L21 + third.L21 + fourth.L21 + fifth.L21 + sixth.L21 + seventh.L21 + eighth.L21;

    result.L20  = first.L20 + second.L20 + third.L20 + fourth.L20 + fifth.L20 + sixth.L20 + seventh.L20 + eighth.L20;

    result.L22  = first.L22 + second.L22 + third.L22 + fourth.L22 + fifth.L22 + sixth.L22 + seventh.L22 + eighth.L22;

    return result;
}

float EvaluateSH(SH sh, vec3 direction, int component) {
    int c = component;

    float result = 0.0;

    result += sh.L00[c] * Y00;

    result += sh.L1_1[c] * Y1_1 * direction.y;
    result += sh.L10[c] * Y10 * direction.z;
    result += sh.L11[c] * Y11 * direction.x;

    result += sh.L2_2[c] * Y2_2 * (direction.x * direction.y);
    result += sh.L2_1[c] * Y2_1 * (direction.y * direction.z);
    result += sh.L21[c] * Y21 * (direction.x * direction.z);
    result += sh.L20[c] * Y20 * (3.0f * direction.z * direction.z - 1.0f);
    result += sh.L22[c] * Y22 * (direction.x * direction.x - direction.y * direction.y);

    return result;
}
