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
        SphericalHarmonics skySH;
        size_t surfelClusterProjectionGroupOffset;
        size_t surfelClusterProjectionGroupCount;
    };

}

#endif /* DiffuseLightProbe_hpp */
