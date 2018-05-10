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

namespace EARenderer {

    class DiffuseLightProbeGenerator {
    protected:
        DiffuseLightProbeData mProbeData;

        const SurfelData *mSurfelData = nullptr;
        const Scene *mScene = nullptr;

        void captureEnvironmentForProbe(const LightProbe& probe);

        float surfelSolidAngle(const Surfel& surfel, const DiffuseLightProbe& probe);

        SurfelClusterProjection projectSurfelCluster(const SurfelCluster& cluster, const DiffuseLightProbe& probe);

        void projectSurfelClustersOnProbe(DiffuseLightProbe& probe);

        std::vector<SphericalHarmonics> surfelProjectionsSH() const;

        std::vector<uint32_t> surfelClusterIndices() const;

        std::vector<uint32_t> probeProjectionsMetadata() const;

    public:
        DiffuseLightProbeData generateProbes(const Scene *scene, const SurfelData& surfelData);
    };

}

#endif /* DiffuseLightProbeGenerator_hpp */
