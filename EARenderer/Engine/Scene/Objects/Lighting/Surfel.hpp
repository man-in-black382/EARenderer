//
//  Surfel.hpp
//  EARenderer
//
//  Created by Pavel Muratov on 12/27/17.
//  Copyright © 2017 MPO. All rights reserved.
//

#ifndef Surfel_hpp
#define Surfel_hpp

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

namespace EARenderer {
    
    struct Surfel {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec3 albedo;
        glm::vec2 uv;
        float area;
        
        Surfel(const glm::vec3& position, const glm::vec3& normal, const glm::vec3& albedo, const glm::vec2& uv, float area);
        Surfel(const glm::vec3& position, const glm::vec3& normal, const glm::vec2& uv);
    };
    
}

#endif /* Surfel_hpp */
