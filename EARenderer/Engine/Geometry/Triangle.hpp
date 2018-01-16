//
//  Triangle.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 26.11.2017.
//  Copyright © 2017 MPO. All rights reserved.
//

#ifndef Triangle_hpp
#define Triangle_hpp

#include <glm/vec3.hpp>
#include <array>

namespace EARenderer {
    
    struct Triangle {
        union {
            struct {
                glm::vec3 a;
                glm::vec3 b;
                glm::vec3 c;
            };

            struct {
                glm::vec3 p1;
                glm::vec3 p2;
                glm::vec3 p3;
            };
            
            glm::vec3 points[3];
            float values[9];
        };
        
        Triangle() = default;
        Triangle(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3);
        
        float area() const;
        std::array<Triangle, 4> split() const;
    };
    
}

#endif /* Triangle_hpp */
