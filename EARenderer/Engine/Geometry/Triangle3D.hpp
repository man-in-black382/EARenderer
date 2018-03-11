//
//  Triangle3D.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 19.01.2018.
//  Copyright © 2018 MPO. All rights reserved.
//

#ifndef Triangle3D_hpp
#define Triangle3D_hpp

#include "Triangle.hpp"

#include <glm/vec3.hpp>

namespace EARenderer {
    
    struct Triangle3D : public Triangle<glm::vec3> {
        using Triangle<glm::vec3>::Triangle;
        
        float area() const override;
        glm::vec3 normal() const;
        std::array<Triangle3D, 4> split() const;
    };
    
}

#endif /* Triangle3D_hpp */
