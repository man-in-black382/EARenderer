//
//  OcclusionMapBuilder.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 09.05.2018.
//  Copyright © 2018 MPO. All rights reserved.
//

#ifndef OcclusionMapBuilder_hpp
#define OcclusionMapBuilder_hpp

#include "Scene.hpp"
#include "Size2D.hpp"

namespace EARenderer {

    class OcclusionMapBuilder {
    private:
        Size2D mMapsResolution;
        std::vector<float> mOcclusionDistances;

        void findOcclusionsDistancesForProbe(size_t probeIndex, Scene *scene);

    public:
        OcclusionMapBuilder(const Size2D& mapResolution);

        void buildLightProbeOcclusionMaps(Scene *scene);
    };

}

#endif /* OcclusionMapBuilder_hpp */
