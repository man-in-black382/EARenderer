//
//  Triangle.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 26.11.2017.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "Triangle.hpp"

namespace EARenderer {
  
#pragma mark - Lifecycle
    
    Triangle::Triangle(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3)
    :
    a(p1), b(p2), c(p3)
    { }
    
}
