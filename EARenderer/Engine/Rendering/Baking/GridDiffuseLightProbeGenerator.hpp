//
//  GridDiffuseLightProbeGenerator.hpp
//  EARenderer
//
//  Created by Pavel Muratov on 5/3/18.
//  Copyright © 2018 MPO. All rights reserved.
//

#ifndef GridDiffuseLightProbeGenerator_hpp
#define GridDiffuseLightProbeGenerator_hpp

#include "DiffuseLightProbeGenerator.hpp"

namespace EARenderer {

    class GridDiffuseLightProbeGenerator: public DiffuseLightProbeGenerator {
    public:
        void generateProbes(Scene *scene) override;
    };

}

#endif /* GridDiffuseLightProbeGenerator_hpp */
