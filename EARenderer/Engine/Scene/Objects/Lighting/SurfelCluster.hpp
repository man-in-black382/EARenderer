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

namespace EARenderer {

    struct SurfelCluster {
        size_t surfelOffset = 0;
        size_t surfelCount = 0;

        SurfelCluster(size_t offset, size_t count);
    };

}

#endif /* SurfelCluster_hpp */
