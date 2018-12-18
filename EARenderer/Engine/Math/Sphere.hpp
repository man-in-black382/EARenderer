//
//  Sphere.hpp
//  EARenderer
//
//  Created by Pavel Muratov on 1/19/18.
//  Copyright © 2018 MPO. All rights reserved.
//

#ifndef Sphere_hpp
#define Sphere_hpp

#include "Triangle3D.hpp"

#include <glm/vec3.hpp>

namespace EARenderer {

    struct Sphere {
        glm::vec3 center;
        float radius;

        Sphere(const glm::vec3 &c, float r);

        bool contains(const glm::vec3 &point) const;

        bool contains(const Triangle3D &triangle) const;
    };

}

#endif /* Sphere_hpp */
