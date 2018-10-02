//
//  DiffuseLightProbe.hpp
//  EARenderer
//
//  Created by Pavel Muratov on 3/15/18.
//  Copyright © 2018 MPO. All rights reserved.
//

#ifndef DiffuseLightProbe_hpp
#define DiffuseLightProbe_hpp

#include "SphericalHarmonics.hpp"

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <bitsery/bitsery.h>
#include "Serializers.h"

namespace EARenderer {

    struct DiffuseLightProbe {
        glm::vec3 position;
        uint32_t surfelClusterProjectionGroupOffset = 0;
        uint32_t surfelClusterProjectionGroupSize = 0;
        SphericalHarmonics skySphericalHarmonics;

        DiffuseLightProbe() = default;
        DiffuseLightProbe(const glm::vec3& position);
    };

    template <typename S>
    void serialize(S& s, DiffuseLightProbe& probe) {
        s.object(probe.position);
        s.value4b(probe.surfelClusterProjectionGroupOffset);
        s.value4b(probe.surfelClusterProjectionGroupSize);
        s.object(probe.skySphericalHarmonics);
    }

}

#endif /* DiffuseLightProbe_hpp */
