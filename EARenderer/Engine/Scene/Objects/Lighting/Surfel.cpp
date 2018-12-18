//
//  Surfel.cpp
//  EARenderer
//
//  Created by Pavel Muratov on 12/27/17.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "Surfel.hpp"

namespace EARenderer {

#pragma mark - Lifecycle

    Surfel::Surfel(const glm::vec3 &position, const glm::vec3 &normal, const Color &albedo, float area)
            :
            position(position),
            normal(normal),
            albedo(albedo),
            area(area) {
    }

    Surfel::Surfel(const glm::vec3 &position, const glm::vec3 &normal)
            :
            Surfel(position, normal, Color(1.0, 1.0), 0.0) {
    }

}
