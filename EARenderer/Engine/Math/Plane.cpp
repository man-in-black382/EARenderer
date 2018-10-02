//
//  Plane.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 11.03.2018.
//  Copyright © 2018 MPO. All rights reserved.
//

#include "Plane.hpp"

#include <glm/geometric.hpp>

namespace EARenderer {

#pragma mark - Lifecycle

    Plane::Plane()
    :
    distance(0),
    normal({ 1.0, 0.0, 0.0 })
    { }

    Plane::Plane(float d, const glm::vec3& n)
    :
    distance(d),
    normal(n)
    { }

    Plane::Plane(const Triangle3D& triangle) {
        normal = triangle.normal();
        distance = glm::dot(triangle.a, normal);
    }

}
