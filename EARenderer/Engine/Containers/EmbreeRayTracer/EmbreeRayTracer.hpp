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
    private:
        RTCDevice mDevice = nullptr;
        RTCScene mScene = nullptr;
        RTCGeometry mGeometry = nullptr;

        static void deviceErrorCallback(void* userPtr, enum RTCError code, const char* str);

    public:
        EmbreeRayTracer(const std::vector<Triangle3D>&& triangles);
        EmbreeRayTracer(const EmbreeRayTracer& that) = default;
        EmbreeRayTracer(EmbreeRayTracer&& that) = default;
        EmbreeRayTracer& operator=(const EmbreeRayTracer& rhs) = default;
        EmbreeRayTracer& operator=(EmbreeRayTracer&& rhs) = default;
        ~EmbreeRayTracer();

        bool lineSegmentOccluded(const glm::vec3& p0, const glm::vec3& p1);
    };

}

#endif /* EmbreeRayTracer_hpp */
