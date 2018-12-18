//
//  Vertex1P1N1UV.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 09.03.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "Vertex1P1N2UV.hpp"

namespace EARenderer {

    Vertex1P1N2UV::Vertex1P1N2UV(const glm::vec4 &position, const glm::vec3 &texCoords,
            const glm::vec2 &lightmapCoords, const glm::vec3 &normal)
            :
            position(position),
            textureCoords(texCoords),
            lightmapCoords(lightmapCoords),
            normal(normal) {
    }

}
