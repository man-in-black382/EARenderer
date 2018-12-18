//
//  Ray.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 11.06.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "Ray3D.hpp"

#include <glm/gtc/matrix_access.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/geometric.hpp>
#include <glm/vec3.hpp>

#include <algorithm>

namespace EARenderer {

#pragma mark - Lifecycle

    Ray3D::Ray3D(const glm::vec3 &origin, const glm::vec3 &direction)
            :
            origin(origin),
            direction(glm::normalize(direction)) {
    }

#pragma mark - Public

    Ray3D Ray3D::transformedBy(const Transformation &t) const {
        return transformedBy(t.modelMatrix());
    }

    Ray3D Ray3D::transformedBy(const glm::mat4 &m) const {
        glm::vec3 newOrigin = m * glm::vec4(origin, 1.0);
        glm::vec3 newDirection = m * glm::vec4(direction, 0.0);
        return Ray3D(newOrigin, newDirection);
    }

}
