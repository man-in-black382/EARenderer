//
//  Triangle3D.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 19.01.2018.
//  Copyright © 2018 MPO. All rights reserved.
//

#include "Triangle3D.hpp"

#include <glm/detail/func_geometric.hpp>

namespace EARenderer {
    
    float Triangle3D::area() const {
        return glm::length(glm::cross(c - a, b - a)) / 2.0f;
    }
    
    std::array<Triangle3D, 4> Triangle3D::split() const {
        glm::vec3 halfAB = (a + b) / 2.0f;
        glm::vec3 halfAC = (a + c) / 2.0f;
        glm::vec3 halfBC = (b + c) / 2.0f;
        
        return {
            Triangle3D(a, halfAB, halfAC),
            Triangle3D(halfAB, b, halfBC),
            Triangle3D(halfBC, c, halfAC),
            Triangle3D(halfAB, halfAC, halfBC)
        };
    }
    
}
