//
//  SurfelCluster.hpp
//  EARenderer
//
//  Created by Pavel Muratov on 3/12/18.
//  Copyright © 2018 MPO. All rights reserved.
//

#ifndef SurfelCluster_hpp
#define SurfelCluster_hpp

#include <stdlib.h>
#include <glm/vec3.hpp>
#include <bitsery/bitsery.h>
#include "Serializers.hpp"

namespace EARenderer {

    struct SurfelCluster {
        uint32_t surfelOffset = 0;
        uint32_t surfelCount = 0;
        glm::vec3 center;

        SurfelCluster() = default;
        SurfelCluster(size_t offset, size_t count);
    };

    template <typename S>
    void serialize(S& s, SurfelCluster& cluster) {
        s.value4b(cluster.surfelOffset);
        s.value4b(cluster.surfelCount);
        s.object(cluster.center);
    }

}

#endif /* SurfelCluster_hpp */
