//
//  Plane.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 11.03.2018.
//  Copyright © 2018 MPO. All rights reserved.
//

#ifndef Plane_hpp
#define Plane_hpp

#include "Triangle3D.hpp"

#include <glm/vec3.hpp>

namespace EARenderer {

    struct Plane {
        float distance;
        glm::vec3 normal;

        Plane();
        Plane(float d, const glm::vec3& n);
        Plane(const Triangle3D& triangle);
    };

}

#endif /* Plane_hpp */
