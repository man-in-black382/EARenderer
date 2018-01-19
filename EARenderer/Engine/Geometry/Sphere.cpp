//
//  Sphere.cpp
//  EARenderer
//
//  Created by Pavel Muratov on 1/19/18.
//  Copyright © 2018 MPO. All rights reserved.
//

#include "Sphere.hpp"

namespace EARenderer {
    
#pragma mark - Lifecycle
    
    Sphere::Sphere(const glm::vec3& c, float r)
    :
    center(c),
    radius(r)
    { }
    
}
