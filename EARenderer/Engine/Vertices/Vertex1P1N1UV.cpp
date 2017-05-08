//
//  Vertex1P1N1UV.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 09.03.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "Vertex1P1N1UV.hpp"

namespace EARenderer {
    
    Vertex1P1N1UV::Vertex1P1N1UV()
    :
    mPosition(glm::vec4()),
    mTextureCoords(glm::vec3()),
    mNormal(glm::vec3())
    { }
    
    Vertex1P1N1UV::Vertex1P1N1UV(glm::vec4 position, glm::vec3 texcoords, glm::vec3 normal)
    :
    mPosition(position),
    mTextureCoords(texcoords),
    mNormal(normal)
    { }
    
}
