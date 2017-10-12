//
//  Vertex1P.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 10.04.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "Vertex1P4.hpp"

namespace EARenderer {
    
    Vertex1P4::Vertex1P4()
    :
    position(glm::vec4())
    { }
    
    Vertex1P4::Vertex1P4(glm::vec4 p)
    :
    position(p)
    { }
    
}
