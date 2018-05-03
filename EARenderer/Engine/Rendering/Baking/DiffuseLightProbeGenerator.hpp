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

namespace EARenderer {

    class DiffuseLightProbeGenerator {
    protected:
        void captureEnvironmentForProbe(Scene *scene, const LightProbe& probe);

        float surfelSolidAngle(Scene *scene, const Surfel& surfel, const DiffuseLightProbe& probe);

        SurfelClusterProjection projectSurfelCluster(Scene *scene, const SurfelCluster& cluster, const DiffuseLightProbe& probe);

        void projectSurfelClustersOnProbe(Scene* scene, DiffuseLightProbe& probe);

    public:
        virtual void generateProbes(Scene *scene) = 0;
    };

}

#endif /* DiffuseLightProbeGenerator_hpp */
