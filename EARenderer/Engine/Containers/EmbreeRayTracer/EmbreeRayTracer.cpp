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

    EmbreeRayTracer::EmbreeRayTracer(const std::vector<Triangle3D>& triangles)
    :
    mDevice(rtcNewDevice(nullptr)),
    mScene(rtcNewScene(mDevice))
    {
        RTCGeometry geometry = rtcNewGeometry(mDevice, RTC_GEOMETRY_TYPE_TRIANGLE);

        glm::vec3 *vertexBuffer = (glm::vec3 *)rtcSetNewGeometryBuffer(geometry,
                                                                       RTC_BUFFER_TYPE_VERTEX,
                                                                       0,
                                                                       RTC_FORMAT_FLOAT3,
                                                                       sizeof(glm::vec3),
                                                                       triangles.size() * 3);

        uint32_t* indexBuffer = (uint32_t *)rtcSetNewGeometryBuffer(geometry,
                                                                    RTC_BUFFER_TYPE_INDEX,
                                                                    0,
                                                                    RTC_FORMAT_UINT3,
                                                                    sizeof(uint32_t) * 3,
                                                                    triangles.size());

        if (vertexBuffer && indexBuffer) {
            for (size_t i = 0; i < triangles.size(); ++i) {
                uint32_t index = (uint32_t)i * 3;
                vertexBuffer[index] = triangles[i].p1;
                vertexBuffer[index + 1] = triangles[i].p2;
                vertexBuffer[index + 2] = triangles[i].p3;

                indexBuffer[index] = index;
                indexBuffer[index + 1] = index + 1;
                indexBuffer[index + 2] = index + 2;
            }
        }

        rtcCommitGeometry(geometry);
        rtcAttachGeometry(mScene, geometry);
        rtcCommitScene(mScene);

        rtcSetSceneFlags(mScene, RTC_SCENE_FLAG_ROBUST);
        deviceErrorCallback(this, rtcGetDeviceError(mDevice), "");
        rtcSetDeviceErrorFunction(mDevice, deviceErrorCallback, this);
    }

    EmbreeRayTracer::EmbreeRayTracer(EmbreeRayTracer&& that) {
        swap(that);
    }

    EmbreeRayTracer::~EmbreeRayTracer() {
        rtcReleaseScene(mScene);
        rtcReleaseDevice(mDevice);
    }

#pragma mark - Operators

    EmbreeRayTracer& EmbreeRayTracer::operator=(EmbreeRayTracer rhs) {
        swap(rhs);
        return *this;
    }

#pragma mark - Swap

    void EmbreeRayTracer::swap(EmbreeRayTracer& that) {
        std::swap(mDevice, that.mDevice);
        std::swap(mScene, that.mScene);
    }

    void swap(EmbreeRayTracer& lhs, EmbreeRayTracer& rhs) {
        lhs.swap(rhs);
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
        ray.mask = -1;

        rtcOccluded1(mScene, &context, &ray);

        // When no intersection is found, the ray data is not updated.
        // In case a hit was found, the tfar component of the ray is set to -inf.
        //
        return ray.tfar < 0.0;
    }

}
