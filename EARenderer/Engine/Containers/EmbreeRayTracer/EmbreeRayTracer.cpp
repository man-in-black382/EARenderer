//
//  EmbreeRayTracer.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 17.03.2018.
//  Copyright © 2018 MPO. All rights reserved.
//

#include "EmbreeRayTracer.hpp"

#include <stdio.h>

namespace EARenderer {

#pragma mark - Lifecycle

    EmbreeRayTracer::EmbreeRayTracer(const std::vector<Triangle3D>&& triangles)
    :
    mDevice(rtcNewDevice(nullptr)),
    mScene(rtcNewScene(mDevice)),
    mGeometry(rtcNewGeometry(mDevice, RTC_GEOMETRY_TYPE_TRIANGLE))
    {
        Triangle3D *bufferPtr = reinterpret_cast<Triangle3D *>(rtcSetNewGeometryBuffer(mGeometry,
                                                                                       RTC_BUFFER_TYPE_VERTEX,
                                                                                       0,
                                                                                       RTC_FORMAT_FLOAT3,
                                                                                       sizeof(Triangle3D),
                                                                                       triangles.size()));

        std::copy(triangles.begin(), triangles.end(), bufferPtr);

        rtcCommitGeometry(mGeometry);
        rtcAttachGeometry(mScene, mGeometry);
        rtcReleaseGeometry(mGeometry);
        rtcCommitScene(mScene);

        rtcSetSceneFlags(mScene, RTC_SCENE_FLAG_ROBUST);
        deviceErrorCallback(this, rtcGetDeviceError(mDevice), "");
        rtcSetDeviceErrorFunction(mDevice, deviceErrorCallback, this);
    }

    EmbreeRayTracer::~EmbreeRayTracer() {
        rtcReleaseScene(mScene);
        rtcReleaseDevice(mDevice);
    }

#pragma mark - Callbacks

    void EmbreeRayTracer::deviceErrorCallback(void* userPtr, enum RTCError code, const char* str) {
        if (code == RTC_ERROR_NONE) { return; }
        printf("Embree device error detected: \ncode: %d, message: %s \n", code, str);
    }

#pragma mark - Occlusion

    bool EmbreeRayTracer::lineSegmentOccluded(const glm::vec3& p0, const glm::vec3& p1) {
        RTCIntersectContext context;
        rtcInitIntersectContext(&context);

        glm::vec3 direction = p1 - p0;
        float distance = glm::length(direction);

        RTCRay ray;
        ray.org_x = p0.x;
        ray.org_y = p0.y;
        ray.org_z = p0.z;
        ray.dir_x = direction.x;
        ray.dir_y = direction.y;
        ray.dir_z = direction.z;
        ray.tnear = 0.f;
        ray.tfar = distance;
        ray.flags = 0;

        rtcOccluded1(mScene, &context, &ray);

        // When no intersection is found, the ray data is not updated.
        // In case a hit was found, the tfar component of the ray is set to -inf.
        //
        return ray.tfar < 0.0;
    }

}
