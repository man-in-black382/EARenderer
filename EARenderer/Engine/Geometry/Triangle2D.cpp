//
//  Triangle2D.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 19.01.2018.
//  Copyright © 2018 MPO. All rights reserved.
//

#include "Triangle2D.hpp"

#include <glm/vec3.hpp>
#include <glm/detail/func_geometric.hpp>

namespace EARenderer {
    
    float Triangle2D::area() const {
        glm::vec3 A(a, 1.0);
        glm::vec3 B(b, 1.0);
        glm::vec3 C(c, 1.0);
        
        return glm::length(glm::cross(C - A, B - A)) / 2.0f;
    }
    
    std::array<Triangle2D, 4> Triangle2D::split() const {
        glm::vec2 halfAB = (a + b) / 2.0f;
        glm::vec2 halfAC = (a + c) / 2.0f;
        glm::vec2 halfBC = (b + c) / 2.0f;
        
        return {
            Triangle2D(a, halfAB, halfAC),
            Triangle2D(halfAB, b, halfBC),
            Triangle2D(halfBC, c, halfAC),
            Triangle2D(halfAB, halfAC, halfBC)
        };
    }
    
}
