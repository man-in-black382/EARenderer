//
//  Triangle.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 26.11.2017.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "Triangle.hpp"

#include <glm/gtx/transform.hpp>

namespace EARenderer {
  
#pragma mark - Lifecycle
    
    Triangle::Triangle(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3)
    :
    a(p1), b(p2), c(p3)
    { }
    
#pragma mark - Math
    
    float Triangle::area() const {
        return glm::length(glm::cross(p2 - p1, p3 - p1)) / 2.0f;
    }
    
}
