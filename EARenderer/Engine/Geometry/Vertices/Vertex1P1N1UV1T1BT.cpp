//
//  Vertex1P1N1UV1T1BT.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 03.09.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "Vertex1P1N1UV1T1BT.hpp"

namespace EARenderer {
    
    Vertex1P1N1UV1T1BT::Vertex1P1N1UV1T1BT(const glm::vec4& position,
                                           const glm::vec3& texcoords,
                                           const glm::vec3& normal,
                                           const glm::vec3& tangent,
                                           const glm::vec3& bitangent)
    :
    Vertex1P1N1UV(position, texcoords, normal),
    tangent(tangent),
    bitangent(bitangent)
    { }
    
}
