//
//  Vertex1P1N1UV.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 07.03.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#ifndef Vertex1P1N1UV_hpp
#define Vertex1P1N1UV_hpp

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

namespace EARenderer {
    
    struct Vertex1P1N1UV {
        glm::vec4 position;
        glm::vec2 textureCoords;
        glm::vec3 normal;
        
        Vertex1P1N1UV();
    };

}

#endif /* Vertex1P1N1UV_hpp */
