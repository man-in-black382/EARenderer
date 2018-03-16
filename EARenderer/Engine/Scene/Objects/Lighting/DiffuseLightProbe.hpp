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

#include <glm/vec3.hpp>

namespace EARenderer {

    struct DiffuseLightProbe {
        glm::vec3 position;
        glm::vec3 normal;
        size_t surfelClusterProjectionGroupOffset = 0;
        size_t surfelClusterProjectionGroupCount = 0;
        SphericalHarmonics skySphericalHarmonics;

        DiffuseLightProbe() = default;
        DiffuseLightProbe(const glm::vec3& position);
        DiffuseLightProbe(const glm::vec3& position, const glm::vec3& normal);
    };

}

#endif /* DiffuseLightProbe_hpp */
