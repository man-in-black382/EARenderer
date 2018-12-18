//
//  Vertex1P.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 10.04.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#ifndef Vertex1P_hpp
#define Vertex1P_hpp

#include <glm/vec4.hpp>

namespace EARenderer {

    struct Vertex1P4 {
        glm::vec4 position;

        Vertex1P4();

        Vertex1P4(glm::vec4 p);
    };

}

#endif /* Vertex1P_hpp */

