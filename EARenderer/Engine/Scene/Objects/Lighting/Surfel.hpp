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
#include <bitsery/bitsery.h>
#include "Serializers.h"
#include "Color.hpp"

namespace EARenderer {
    
    struct Surfel {
        glm::vec3 position;
        glm::vec3 normal;
        Color albedo;
        float area = 0;

        Surfel() = default;
        Surfel(const glm::vec3& position, const glm::vec3& normal, const Color& albedo, float area);
        Surfel(const glm::vec3& position, const glm::vec3& normal);
    };

    template <typename S>
    void serialize(S& s, Surfel& surfel) {
        s.object(surfel.position);
        s.object(surfel.normal);
        s.object(surfel.albedo);
        s.value4b(surfel.area);
    }
    
}

#endif /* Surfel_hpp */
