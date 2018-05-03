//
//  DiffuseLightProbeGenerator.cpp
//  EARenderer
//
//  Created by Pavel Muratov on 5/3/18.
//  Copyright © 2018 MPO. All rights reserved.
//

#include "DiffuseLightProbeGenerator.hpp"

namespace EARenderer {

    float DiffuseLightProbeGenerator::surfelSolidAngle(Scene *scene, const Surfel& surfel, const DiffuseLightProbe& probe) {
        glm::vec3 Wps = surfel.position - probe.position;
        float distance2 = glm::length2(Wps);
        Wps = glm::normalize(Wps);

        float distanceTerm = std::min(surfel.area / distance2, 1.f);

        float visibilityTerm = std::max(glm::dot(-surfel.normal, Wps), 0.f);

        // Used to indicate whether surfel is located in the positive hemisphere of static geometry probe
        // to reduce ray casts. For full-sphere (dynamic geometry) grid probes it is always true.
        bool isInSameHemisphere = true;

        if (probe.normal.x != 0.0 || probe.normal.y != 0.0 || probe.normal.z != 0.0) {
            isInSameHemisphere = glm::dot(Wps, probe.normal) > 0.0;
        }

        float visibilityTest = 0.0;

        // Save ray casts if surfel's facing away from the standpoint
        if (visibilityTerm > 0.0 && isInSameHemisphere) {
            visibilityTest = scene->rayTracer()->lineSegmentOccluded(probe.position, surfel.position) ? 0.0 : 1.0;
        }

        return distanceTerm * visibilityTerm * visibilityTest;
    }

    SurfelClusterProjection DiffuseLightProbeGenerator::projectSurfelCluster(Scene *scene, const SurfelCluster& cluster, const DiffuseLightProbe& probe) {
        SurfelClusterProjection projection;

        for (size_t i = cluster.surfelOffset; i < cluster.surfelOffset + cluster.surfelCount; i++) {
            Surfel& surfel = scene->surfels()[i];
            glm::vec3 Wps_norm = glm::normalize(surfel.position - probe.position);
            float solidAngle = surfelSolidAngle(scene, surfel, probe);

            if (solidAngle > 0.0) {
                projection.sphericalHarmonics.contribute(Wps_norm, surfel.albedo, solidAngle);
            }
        }

        projection.sphericalHarmonics.normalize();

        return projection;
    }

    void DiffuseLightProbeGenerator::projectSurfelClustersOnProbe(Scene* scene, DiffuseLightProbe& probe) {
        probe.surfelClusterProjectionGroupOffset = scene->surfelClusterProjections().size();

        for (size_t i = 0; i < scene->surfelClusters().size(); i++) {
            SurfelCluster &cluster = scene->surfelClusters()[i];
            SurfelClusterProjection projection = projectSurfelCluster(scene, cluster, probe);

            // Only accept projections with non-zero SH
            if (projection.sphericalHarmonics.magnitude2() > 10e-09) {
                projection.surfelClusterIndex = i;
                scene->surfelClusterProjections().push_back(projection);
                probe.surfelClusterProjectionGroupCount++;
            }
        }
    }

}
