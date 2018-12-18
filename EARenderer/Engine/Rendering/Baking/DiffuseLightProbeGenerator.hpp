//
//  DiffuseLightProbeGenerator.hpp
//  EARenderer
//
//  Created by Pavel Muratov on 5/3/18.
//  Copyright © 2018 MPO. All rights reserved.
//

#ifndef DiffuseLightProbeGenerator_hpp
#define DiffuseLightProbeGenerator_hpp

#include "Scene.hpp"
#include "DiffuseLightProbeData.hpp"
#include "SurfelData.hpp"

#include <memory>
#include <glm/vec2.hpp>

namespace EARenderer {

    class DiffuseLightProbeGenerator {
    private:

#pragma mark - Member variables

        std::shared_ptr<DiffuseLightProbeData> mProbeData;
        std::shared_ptr<const SurfelData> mSurfelData;
        const Scene *mScene = nullptr;

#pragma mark - Members functions

        float surfelSolidAngle(const Surfel &surfel, const DiffuseLightProbe &probe);

        SurfelClusterProjection projectSurfelCluster(const SurfelCluster &cluster, const DiffuseLightProbe &probe);

        void projectSurfelClustersOnProbe(DiffuseLightProbe &probe);

        void projectSkyOnProbe(DiffuseLightProbe &probe);

    public:
        std::shared_ptr<DiffuseLightProbeData> generateProbes(const Scene *scene, std::shared_ptr<const SurfelData> surfelData);
    };

}

#endif /* DiffuseLightProbeGenerator_hpp */
