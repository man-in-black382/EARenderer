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

SH DebugSH() {
    SH sh; // Blue in YCoCg space
    sh.L00  = vec3(2.78416443, -5.56832886, -2.78416443);
    sh.L11  = vec3(-1.85610926, 3.71221852, 1.85610926);
    sh.L10  = vec3(1.85610926, -3.71221852, -1.85610926);
    sh.L1_1 = vec3(-1.85610926, 3.71221852, 1.85610926);
    sh.L21  = vec3(-0.898585081, 1.79717016, 0.898585081);
    sh.L2_1 = vec3(-0.898585081, 1.79717016, 0.898585081);
    sh.L2_2 = vec3(0.898585081, -1.79717016, -0.898585081);
    sh.L20  = vec3(-0.0000000463841872, 0.0000000927683743, 0.0000000463841872);
    sh.L22  = vec3(0.0, 0.0, 0.0);
    return sh;
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

// Performs the SH triple product r = a * b
// From John Snyder
//
// https://github.com/Patapom/GodComplex/blob/master/Resources/Shaders/Inc/SH.hlsl
// http://www.patapom.com/blog/SHPortal/
//
SH SHProduct(SH first, SH second) {

    vec3 a[9], b[9], r[9];

    // First
    a[0] = first.L00;

    a[1] = first.L1_1;
    a[2] = first.L10;
    a[3] = first.L11;

    a[4] = first.L2_2;
    a[5] = first.L2_1;
    a[6] = first.L20;
    a[7] = first.L21;
    a[8] = first.L22;

    // Second
    b[0] = second.L00;

    b[1] = second.L1_1;
    b[2] = second.L10;
    b[3] = second.L11;

    b[4] = second.L2_2;
    b[5] = second.L2_1;
    b[6] = second.L20;
    b[7] = second.L21;
    b[8] = second.L22;

    vec3 ta, tb, t;

    // The convolution's Clebsch-Gordan coefficients were precomputed
    const float    C0 = 0.282094792935999980;
    const float    C1 = -0.126156626101000010;
    const float    C2 = 0.218509686119999990;
    const float    C3 = 0.252313259986999990;
    const float    C4 = 0.180223751576000010;
    const float    C5 = 0.156078347226000000;
    const float    C6 = 0.090111875786499998;

    // [0,0]: 0,
    r[0] = C0*a[0]*b[0];

    // [1,1]: 0,6,8,
    ta = C0*a[0]+C1*a[6]-C2*a[8];
    tb = C0*b[0]+C1*b[6]-C2*b[8];
    r[1] = ta*b[1]+tb*a[1];
    t = a[1]*b[1];
    r[0] += C0*t;
    r[6] = C1*t;
    r[8] = -C2*t;

    // [1,2]: 5,
    ta = C2*a[5];
    tb = C2*b[5];
    r[1] += ta*b[2]+tb*a[2];
    r[2] = ta*b[1]+tb*a[1];
    t = a[1]*b[2]+a[2]*b[1];
    r[5] = C2*t;

    // [1,3]: 4,
    ta = C2*a[4];
    tb = C2*b[4];
    r[1] += ta*b[3]+tb*a[3];
    r[3] = ta*b[1]+tb*a[1];
    t = a[1]*b[3]+a[3]*b[1];
    r[4] = C2*t;

    // [2,2]: 0,6,
    ta = C0*a[0]+C3*a[6];
    tb = C0*b[0]+C3*b[6];
    r[2] += ta*b[2]+tb*a[2];
    t = a[2]*b[2];
    r[0] += C0*t;
    r[6] += C3*t;

    // [2,3]: 7,
    ta = C2*a[7];
    tb = C2*b[7];
    r[2] += ta*b[3]+tb*a[3];
    r[3] += ta*b[2]+tb*a[2];
    t = a[2]*b[3]+a[3]*b[2];
    r[7] = C2*t;

    // [3,3]: 0,6,8,
    ta = C0*a[0]+C1*a[6]+C2*a[8];
    tb = C0*b[0]+C1*b[6]+C2*b[8];
    r[3] += ta*b[3]+tb*a[3];
    t = a[3]*b[3];
    r[0] += C0*t;
    r[6] += C1*t;
    r[8] += C2*t;

    // [4,4]: 0,6,
    ta = C0*a[0]-C4*a[6];
    tb = C0*b[0]-C4*b[6];
    r[4] += ta*b[4]+tb*a[4];
    t = a[4]*b[4];
    r[0] += C0*t;
    r[6] -= C4*t;

    // [4,5]: 7,
    ta = C5*a[7];
    tb = C5*b[7];
    r[4] += ta*b[5]+tb*a[5];
    r[5] += ta*b[4]+tb*a[4];
    t = a[4]*b[5]+a[5]*b[4];
    r[7] += C5*t;

    // [5,5]: 0,6,8,
    ta = C0*a[0]+C6*a[6]-C5*a[8];
    tb = C0*b[0]+C6*b[6]-C5*b[8];
    r[5] += ta*b[5]+tb*a[5];
    t = a[5]*b[5];
    r[0] += C0*t;
    r[6] += C6*t;
    r[8] -= C5*t;

    // [6,6]: 0,6,
    ta = C0*a[0];
    tb = C0*b[0];
    r[6] += ta*b[6]+tb*a[6];
    t = a[6]*b[6];
    r[0] += C0*t;
    r[6] += C4*t;

    // [7,7]: 0,6,8,
    ta = C0*a[0]+C6*a[6]+C5*a[8];
    tb = C0*b[0]+C6*b[6]+C5*b[8];
    r[7] += ta*b[7]+tb*a[7];
    t = a[7]*b[7];
    r[0] += C0*t;
    r[6] += C6*t;
    r[8] += C5*t;

    // [8,8]: 0,6,
    ta = C0*a[0]-C4*a[6];
    tb = C0*b[0]-C4*b[6];
    r[8] += ta*b[8]+tb*a[8];
    t = a[8]*b[8];
    r[0] += C0*t;
    r[6] -= C4*t;
    // entry count=13
    // multiply count=120
    // addition count=74

    SH result;

    result.L00 = r[0];

    result.L1_1 = r[1];
    result.L10 = r[2];
    result.L11 = r[3];

    result.L2_2 = r[4];
    result.L2_1 = r[5];
    result.L20 = r[6];
    result.L21 = r[7];
    result.L22 = r[8];

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

float SHMagnitude2(SH sh) {
    return dot(sh.L00, sh.L00) +

    dot(sh.L1_1, sh.L1_1) +
    dot(sh.L10, sh.L10) +
    dot(sh.L11, sh.L11) +

    dot(sh.L2_2, sh.L2_2) +
    dot(sh.L2_1, sh.L2_1) +
    dot(sh.L20, sh.L20) +
    dot(sh.L21, sh.L21) +
    dot(sh.L22, sh.L22);
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
