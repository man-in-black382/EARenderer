#include "Geometry.glsl"

int CubeMapFace(vec3 sampleVector) {
    int majorAxis = 0;
    vec3 a = abs(sampleVector);

    if (a.y >= a.x && a.y >= a.z) {
        majorAxis = 1;
    } else if (a.z >= a.x && a.z >= a.y) {
        majorAxis = 2;
    }

    if (sampleVector[majorAxis] < 0.0) {
        majorAxis = majorAxis * 2 + 1;
    } else {
        majorAxis *= 2;
    }

    return majorAxis;
}

vec3 CubeMapTextureCoords(vec3 sampleVector) {
    vec3 a = abs(sampleVector);
    float tmpS = 0.0, tmpT = 0.0, m = 0.0, face = 0.0;

    switch (CubeMapFace(sampleVector)) {
    /* +X */ case 0: tmpS = -sampleVector.z; tmpT = -sampleVector.y; m = a.x; face = 0.0; break;
    /* -X */ case 1: tmpS =  sampleVector.z; tmpT = -sampleVector.y; m = a.x; face = 1.0; break;
    /* +Y */ case 2: tmpS =  sampleVector.x; tmpT =  sampleVector.z; m = a.y; face = 2.0; break;
    /* -Y */ case 3: tmpS =  sampleVector.x; tmpT = -sampleVector.z; m = a.y; face = 3.0; break;
    /* +Z */ case 4: tmpS =  sampleVector.x; tmpT = -sampleVector.y; m = a.z; face = 4.0; break;
    /* -Z */ case 5: tmpS = -sampleVector.x; tmpT = -sampleVector.y; m = a.z; face = 5.0; break;
    }

    return vec3(0.5 * (tmpS / m + 1.0), 0.5 * (tmpT / m + 1.0), face);
}

mat4 ViewMatrixForCubeFace(vec3 eye, int face) {
    vec3 direction;
    vec3 up;

    switch (face) {
        case 0:
            direction = vec3(1.0, 0.0, 0.0);
            up = vec3(0.0, -1.0, 0.0);
            break;
        case 1:
            direction = vec3(-1.0, 0.0, 0.0);
            up = vec3(0.0, -1.0, 0.0);
            break;
        case 2:
             direction = vec3(0.0, 1.0, 0.0);
             up = vec3(0.0, 0.0, 1.0);
             break;
        case 3:
            direction = vec3(0.0, -1.0, 0.0);
            up = vec3(0.0, 0.0, -1.0);
            break;
        case 4:
            direction = vec3(0.0, 0.0, 1.0);
            up = vec3(0.0, -1.0, 0.0);
            break;
        case 5:
            direction = vec3(0.0, 0.0, -1.0);
            up = vec3(0.0, -1.0, 0.0);
            break;
        default:
            direction = vec3(0.0);
            up = vec3(0.0, -1.0, 0.0);
    }

    return LookAt(eye, eye + direction, up);
}