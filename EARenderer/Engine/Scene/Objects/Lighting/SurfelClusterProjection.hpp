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

namespace EARenderer {

    struct SurfelClusterProjection {
        size_t surfelClusterIndex;
        SphericalHarmonics sh;
    };

}

#endif /* SurfelClusterProjection_hpp */
