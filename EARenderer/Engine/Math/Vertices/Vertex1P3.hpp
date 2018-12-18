//
//  Vertex1P3.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 21.06.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#ifndef Vertex1P3_hpp
#define Vertex1P3_hpp

#include <glm/vec3.hpp>

namespace EARenderer {

    struct Vertex1P3 {
        glm::vec3 position;

        Vertex1P3();

        Vertex1P3(glm::vec3 p);
    };

}

#endif /* Vertex1P3_hpp */
