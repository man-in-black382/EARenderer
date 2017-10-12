//
//  Vertex1P1N1UV1T1BT.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 03.09.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#ifndef Vertex1P1N1UV1T1BT_hpp
#define Vertex1P1N1UV1T1BT_hpp

#include "Vertex1P1N1UV.hpp"

namespace EARenderer {
    
    /**
     1 position
     1 normal
     1 texture coordinate
     1 tangent vector
     1 bitangent vector
     */
    struct Vertex1P1N1UV1T1BT: Vertex1P1N1UV {
        glm::vec3 tangent;
        glm::vec3 bitangent;
        
        using Vertex1P1N1UV::Vertex1P1N1UV;
        Vertex1P1N1UV1T1BT(const glm::vec4& position,
                           const glm::vec3& texcoords,
                           const glm::vec3& normal,
                           const glm::vec3& tangent,
                           const glm::vec3& bitangent);
    };
    
}

#endif /* Vertex1P1N1UV1T1BT_hpp */
