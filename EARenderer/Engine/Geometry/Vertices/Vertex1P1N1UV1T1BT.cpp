//
//  Vertex1P1N1UV1T1BT.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 03.09.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "Vertex1P1N1UV1T1BT.hpp"

namespace EARenderer {
    
#pragma mark - Lifecycle
    
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
    
#pragma mark - Transformations
    
    Vertex1P1N1UV1T1BT Vertex1P1N1UV1T1BT::transformedBy(const Transformation& t) {
        glm::mat4 modelMatrix = t.modelMatrix();
        glm::mat4 normalMatrix = glm::transpose(glm::inverse(modelMatrix));
        
        return Vertex1P1N1UV1T1BT(modelMatrix * position,
                                  textureCoords,
                                  normalMatrix * glm::vec4(normal, 1.0),
                                  normalMatrix * glm::vec4(tangent, 1.0),
                                  normalMatrix * glm::vec4(bitangent, 1.0));
    }
    
}
