//
//  EmbreeRayTracer.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 17.03.2018.
//  Copyright © 2018 MPO. All rights reserved.
//

#ifndef EmbreeRayTracer_hpp
#define EmbreeRayTracer_hpp

#include "Ray3D.hpp"
#include "Triangle3D.hpp"

#include <vector>
#include <glm/vec3.hpp>
#include <rtcore.h>

namespace EARenderer {

    class EmbreeRayTracer {
    public:
        enum class FaceFilter { None, CullFront, CullBack };

    private:
        RTCDevice mDevice = nullptr;
        RTCScene mScene = nullptr;

        FaceFilter mFaceFilter;

        static void deviceErrorCallback(void* userPtr, enum RTCError code, const char* str);
        static void intersectionFilter(const struct RTCFilterFunctionNArguments* args);
        static void occlusionFilter(const struct RTCFilterFunctionNArguments* args);

    public:
        EmbreeRayTracer(const std::vector<Triangle3D>& triangles);
        EmbreeRayTracer(const EmbreeRayTracer& that) = delete;
        EmbreeRayTracer(EmbreeRayTracer&& that);
        EmbreeRayTracer& operator=(EmbreeRayTracer rhs);
        ~EmbreeRayTracer();

        void swap(EmbreeRayTracer& that);

        bool lineSegmentOccluded(const glm::vec3& p0, const glm::vec3& p1, FaceFilter faceFilter = FaceFilter::None);
        bool rayHit(const Ray3D& ray, float& distance, FaceFilter faceFilter = FaceFilter::None);
    };

    void swap(EmbreeRayTracer& lhs, EmbreeRayTracer& rhs);
}

#endif /* EmbreeRayTracer_hpp */
