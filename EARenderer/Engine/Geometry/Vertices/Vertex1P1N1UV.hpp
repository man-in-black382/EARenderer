//
//  Vertex1P1N1UV.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 07.03.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#ifndef Vertex1P1N1UV_hpp
#define Vertex1P1N1UV_hpp

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

namespace EARenderer {
    
    /**
     1 position
     1 normal
     1 texture coordinate
     */
    struct Vertex1P1N1UV {
        glm::vec4 position;
        glm::vec3 textureCoords;
        glm::vec3 normal;
        
        Vertex1P1N1UV(const glm::vec4& position,
                      const glm::vec3& texcoords,
                      const glm::vec3& normal);
    };

}

#endif /* Vertex1P1N1UV_hpp */
