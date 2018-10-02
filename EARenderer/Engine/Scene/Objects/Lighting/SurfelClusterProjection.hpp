//
//  SurfelClusterProjection.hpp
//  EARenderer
//
//  Created by Pavel Muratov on 3/15/18.
//  Copyright © 2018 MPO. All rights reserved.
//

#ifndef SurfelClusterProjection_hpp
#define SurfelClusterProjection_hpp

#include "SphericalHarmonics.hpp"

#include <bitsery/bitsery.h>

namespace EARenderer {

    struct SurfelClusterProjection {
        uint32_t surfelClusterIndex = 0;
        SphericalHarmonics sphericalHarmonics;
    };

    template <typename S>
    void serialize(S& s, SurfelClusterProjection& projection) {
        s.value4b(projection.surfelClusterIndex);
        s.object(projection.sphericalHarmonics);
    }

}

#endif /* SurfelClusterProjection_hpp */
