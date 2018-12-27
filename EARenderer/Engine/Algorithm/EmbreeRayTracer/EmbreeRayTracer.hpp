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
        enum class FaceFilter {
            None, CullFront, CullBack
        };

    private:
        RTCDevice mDevice = nullptr;
        RTCScene mScene = nullptr;

        FaceFilter mFaceFilter;

        static void deviceErrorCallback(void *userPtr, enum RTCError code, const char *str);

        static void intersectionFilter(const struct RTCFilterFunctionNArguments *args);

        static void occlusionFilter(const struct RTCFilterFunctionNArguments *args);

    public:
        EmbreeRayTracer(const std::vector<Triangle3D> &triangles);

        EmbreeRayTracer(const EmbreeRayTracer &that) = delete;

        EmbreeRayTracer(EmbreeRayTracer &&that);

        EmbreeRayTracer &operator=(EmbreeRayTracer rhs);

        ~EmbreeRayTracer();

        void swap(EmbreeRayTracer &that);

        ///
        /// @param p0 line segment start
        /// @param p1 line segment end
        /// @param p0OffsetFactor moves starting point in the direction of ending point
        /// to avoid possible collisions due to floating point errors (if such collisions are expected).
        /// Value is expected to be in range of [0; 1]
        /// @param p1OffsetFactor moves ending point in the direction of starting point
        /// to avoid possible collisions due to floating point errors (if such collisions are expected)
        /// Value is expected to be in range of [0; 1]
        /// @param FaceFilter indicates which faces should be ignored during ray tracing
        /// @return flag indicating whether line segment is occluded
        bool lineSegmentOccluded(
                const glm::vec3 &p0,
                const glm::vec3 &p1,
                float p0OffsetFactor = 0.0f,
                float p1OffsetFactor = 0.0f,
                FaceFilter faceFilter = FaceFilter::None
        );

        bool rayHit(const Ray3D &ray, float &distance, FaceFilter faceFilter = FaceFilter::None);
    };

    void swap(EmbreeRayTracer &lhs, EmbreeRayTracer &rhs);
}

#endif /* EmbreeRayTracer_hpp */
