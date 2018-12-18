//
//  Serializers.h
//  EARenderer
//
//  Created by Pavlo Muratov on 29.09.2018.
//  Copyright © 2018 MPO. All rights reserved.
//

#ifndef Serializers_h
#define Serializers_h

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include "Color.hpp"

namespace glm {

    template<typename S>
    void serialize(S &s, glm::vec3 &v) {
        s.value4b(v.x);
        s.value4b(v.y);
        s.value4b(v.z);
    }

    template<typename S>
    void serialize(S &s, glm::vec4 &v) {
        s.value4b(v.x);
        s.value4b(v.y);
        s.value4b(v.z);
        s.value4b(v.w);
    }

    template<typename S>
    void serialize(S &s, glm::ivec3 &v) {
        s.value4b(v.x);
        s.value4b(v.y);
        s.value4b(v.z);
    }

}

#endif /* Serializers_h */
