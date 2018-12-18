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

    EmbreeRayTracer::EmbreeRayTracer(const std::vector<Triangle3D> &triangles)
            :
            mDevice(rtcNewDevice(nullptr)),
            mScene(rtcNewScene(mDevice)) {
        RTCGeometry geometry = rtcNewGeometry(mDevice, RTC_GEOMETRY_TYPE_TRIANGLE);

        glm::vec3 *vertexBuffer = (glm::vec3 *) rtcSetNewGeometryBuffer(geometry,
                RTC_BUFFER_TYPE_VERTEX,
                0,
                RTC_FORMAT_FLOAT3,
                sizeof(glm::vec3),
                triangles.size() * 3);

        glm::uvec3 *indexBuffer = (glm::uvec3 *) rtcSetNewGeometryBuffer(geometry,
                RTC_BUFFER_TYPE_INDEX,
                0,
                RTC_FORMAT_UINT3,
                sizeof(glm::uvec3),
                triangles.size());

        if (vertexBuffer && indexBuffer) {
            for (uint32_t i = 0; i < triangles.size(); ++i) {
                uint32_t index = i * 3;
                vertexBuffer[index] = glm::vec3(triangles[i].p1);
                vertexBuffer[index + 1] = glm::vec3(triangles[i].p2);
                vertexBuffer[index + 2] = glm::vec3(triangles[i].p3);

                indexBuffer[i].x = index;
                indexBuffer[i].y = index + 1;
                indexBuffer[i].z = index + 2;
            }
        }

        rtcSetGeometryUserData(geometry, this);
        rtcSetGeometryIntersectFilterFunction(geometry, intersectionFilter);
        rtcSetGeometryOccludedFilterFunction(geometry, intersectionFilter);

        rtcCommitGeometry(geometry);
        rtcAttachGeometry(mScene, geometry);
        rtcCommitScene(mScene);
        rtcReleaseGeometry(geometry);

        rtcSetSceneFlags(mScene, RTC_SCENE_FLAG_ROBUST);
        deviceErrorCallback(this, rtcGetDeviceError(mDevice), "");
        rtcSetDeviceErrorFunction(mDevice, deviceErrorCallback, this);
    }

    EmbreeRayTracer::EmbreeRayTracer(EmbreeRayTracer &&that) {
        swap(that);
    }

    EmbreeRayTracer::~EmbreeRayTracer() {
        rtcReleaseScene(mScene);
        rtcReleaseDevice(mDevice);
    }

#pragma mark - Operators

    EmbreeRayTracer &EmbreeRayTracer::operator=(EmbreeRayTracer rhs) {
        swap(rhs);
        return *this;
    }

#pragma mark - Swap

    void EmbreeRayTracer::swap(EmbreeRayTracer &that) {
        std::swap(mDevice, that.mDevice);
        std::swap(mScene, that.mScene);
    }

    void swap(EmbreeRayTracer &lhs, EmbreeRayTracer &rhs) {
        lhs.swap(rhs);
    }

#pragma mark - Callbacks

    void EmbreeRayTracer::deviceErrorCallback(void *userPtr, enum RTCError code, const char *str) {
        if (code == RTC_ERROR_NONE) {return;}
        printf("Embree device error detected: \ncode: %d, message: %s \n", code, str);
    }

    void EmbreeRayTracer::intersectionFilter(const struct RTCFilterFunctionNArguments *args) {
        EmbreeRayTracer *thisPtr = reinterpret_cast<EmbreeRayTracer *>(args->geometryUserPtr);

        if (thisPtr->mFaceFilter == FaceFilter::None) return;

        glm::vec3 triangleNormal(RTCHitN_Ng_x(args->hit, args->N, 0),
                RTCHitN_Ng_y(args->hit, args->N, 0),
                RTCHitN_Ng_z(args->hit, args->N, 0));

        glm::vec3 rayDirection(RTCRayN_dir_x(args->ray, args->N, 0),
                RTCRayN_dir_y(args->ray, args->N, 0),
                RTCRayN_dir_z(args->ray, args->N, 0));

        float dot = glm::dot(triangleNormal, rayDirection);
        bool vectorsPointingInSameHemisphere = dot > 0.0;

        switch (thisPtr->mFaceFilter) {
            case FaceFilter::CullFront:
                args->valid[0] = vectorsPointingInSameHemisphere ? -1 : 0;
                break;
            case FaceFilter::CullBack:
                args->valid[0] = vectorsPointingInSameHemisphere ? 0 : -1;
                break;
            default:
                break;
        }
    }

#pragma mark - Occlusion

    bool EmbreeRayTracer::lineSegmentOccluded(const glm::vec3 &p0, const glm::vec3 &p1, FaceFilter faceFilter) {
        RTCIntersectContext context;
        rtcInitIntersectContext(&context);

        mFaceFilter = faceFilter;

        glm::vec3 direction = (p1 - p0);

        RTCRay ray;
        ray.org_x = p0.x;
        ray.org_y = p0.y;
        ray.org_z = p0.z;
        ray.dir_x = direction.x;
        ray.dir_y = direction.y;
        ray.dir_z = direction.z;
        ray.tnear = 0.00;
        ray.tfar = 1.0;
        ray.flags = 0;

        rtcOccluded1(mScene, &context, &ray);

        // When no intersection is found, the ray data is not updated.
        // In case a hit was found, the tfar component of the ray is set to -inf.
        //
        return ray.tfar < 0.0;
    }

    bool EmbreeRayTracer::rayHit(const Ray3D &ray, float &distance, FaceFilter faceFilter) {
        RTCIntersectContext context;
        rtcInitIntersectContext(&context);

        mFaceFilter = faceFilter;

        RTCRayHit rayHit;

        rayHit.ray.org_x = ray.origin.x;
        rayHit.ray.org_y = ray.origin.y;
        rayHit.ray.org_z = ray.origin.z;
        rayHit.ray.dir_x = ray.direction.x;
        rayHit.ray.dir_y = ray.direction.y;
        rayHit.ray.dir_z = ray.direction.z;
        rayHit.ray.tnear = 0.00f;
        rayHit.ray.tfar = std::numeric_limits<float>::max();
        rayHit.ray.flags = 0;
        rayHit.ray.mask = -1;

        rayHit.hit.instID[0] = RTC_INVALID_GEOMETRY_ID;
        rayHit.hit.geomID = RTC_INVALID_GEOMETRY_ID;

        rtcIntersect1(mScene, &context, &rayHit);

        distance = rayHit.ray.tfar;

        return rayHit.hit.geomID != RTC_INVALID_GEOMETRY_ID;
    }

}
