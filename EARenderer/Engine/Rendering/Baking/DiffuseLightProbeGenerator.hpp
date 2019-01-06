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
        std::unique_ptr<DiffuseLightProbeData> mProbeData;

        float surfelSolidAngle(const Surfel &surfel, const DiffuseLightProbe &probe, const Scene &scene);

        SurfelClusterProjection projectSurfelCluster(const SurfelCluster &cluster, const DiffuseLightProbe &probe, const SurfelData& surfelData, const Scene &scene);

        void projectSurfelClustersOnProbe(DiffuseLightProbe &probe, const SurfelData& surfelData, const Scene &scene);

        void projectSkyOnProbe(DiffuseLightProbe &probe, const Scene &scene);

    public:
        std::unique_ptr<DiffuseLightProbeData> generateProbes(const Scene &scene, const SurfelData& surfelData);
    };

}

#endif /* DiffuseLightProbeGenerator_hpp */
