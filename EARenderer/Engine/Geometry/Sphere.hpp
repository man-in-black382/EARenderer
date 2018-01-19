//
//  Sphere.hpp
//  EARenderer
//
//  Created by Pavel Muratov on 1/19/18.
//  Copyright © 2018 MPO. All rights reserved.
//

#ifndef Sphere_hpp
#define Sphere_hpp

#include <glm/vec3.hpp>

namespace EARenderer {
    
    struct Sphere {
        glm::vec3 center;
        float radius;
        
        Sphere(const glm::vec3& c, float r);
    };
    
}

#endif /* Sphere_hpp */
