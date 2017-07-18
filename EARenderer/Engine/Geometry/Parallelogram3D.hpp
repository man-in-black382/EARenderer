//
//  Parallelogram3D.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 01.07.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#ifndef Parallelogram3D_hpp
#define Parallelogram3D_hpp

#include "Transformation.hpp"

namespace EARenderer {
    
    struct Parallelogram3D {
        glm::vec3 corner;
        glm::vec3 side1;
        glm::vec3 side2;
        
        Parallelogram3D(const glm::vec3& corner, const glm::vec3& sideVector1, const glm::vec3& sideVector2);
        
        glm::vec3 normal() const;
        
        Parallelogram3D transformedBy(const Transformation& t) const;
        Parallelogram3D transformedBy(const glm::mat4& mat) const;
    };
    
}

#endif /* Parallelogram3D_hpp */
