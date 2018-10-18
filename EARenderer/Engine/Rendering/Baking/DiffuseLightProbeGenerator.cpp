//
//  DiffuseLightProbeGenerator.cpp
//  EARenderer
//
//  Created by Pavel Muratov on 5/3/18.
//  Copyright © 2018 MPO. All rights reserved.
//

#include "DiffuseLightProbeGenerator.hpp"
#include "Measurement.hpp"

namespace EARenderer {

#pragma mark - Protected

    float DiffuseLightProbeGenerator::surfelSolidAngle(const Surfel& surfel, const DiffuseLightProbe& probe) {
        glm::vec3 Wps = surfel.position - probe.position;
        float distance2 = glm::length2(Wps);
        Wps = glm::normalize(Wps);

        float distanceTerm = std::min(surfel.area / distance2, 1.f);

        float visibilityTerm = std::max(glm::dot(-surfel.normal, Wps), 0.f);

        float visibilityTest = 0.0;

        // Save ray casts if surfel's facing away from the standpoint
        if (visibilityTerm > 0.0) {
            visibilityTest = mScene->rayTracer()->lineSegmentOccluded(probe.position, surfel.position) ? 0.0 : 1.0;
        }

        return distanceTerm * visibilityTerm * visibilityTest;
    }

    SurfelClusterProjection DiffuseLightProbeGenerator::projectSurfelCluster(const SurfelCluster& cluster, const DiffuseLightProbe& probe) {
        SurfelClusterProjection projection;

        for (size_t i = cluster.surfelOffset; i < cluster.surfelOffset + cluster.surfelCount; i++) {
            const Surfel& surfel = mSurfelData->surfels()[i];
            glm::vec3 Wps_norm = glm::normalize(surfel.position - probe.position);
            float solidAngle = surfelSolidAngle(surfel, probe);

            if (solidAngle > 0.0) {
                // Accumulating in YCoCg space to enable compression possibilities
                projection.sphericalHarmonics.contribute(Wps_norm, surfel.albedo.YCoCg(), solidAngle);
            }
        }

//        // FIXME: DEBUG
//        if (mProbeData->mSurfelClusterProjections.size() == 0) {
//            SphericalHarmonics debugSH;
//            debugSH.contribute(glm::vec3(1.0, 0.0, 0.0), glm::vec3(1.0, 0.0, 0.0), 2 * M_PI);
//            debugSH.contribute(glm::vec3(-1.0, 0.0, 0.0), glm::vec3(1.0, 0.0, 0.0), 2 * M_PI);
//             projection.sphericalHarmonics = debugSH;
//        } else if (mProbeData->mSurfelClusterProjections.size() == 1) {
//            SphericalHarmonics debugSH;
//            debugSH.contribute(glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0), 2 * M_PI);
//            debugSH.contribute(glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0), 2 * M_PI);
//            projection.sphericalHarmonics = debugSH;
//        } else if (mProbeData->mSurfelClusterProjections.size() == 2) {
//            SphericalHarmonics debugSH;
//            debugSH.contribute(glm::vec3(1.0, 0.0, 0.0), glm::vec3(1.0, 1.0, 0.0), 2 * M_PI);
//            debugSH.contribute(glm::vec3(-1.0, 0.0, 0.0), glm::vec3(1.0, 1.0, 0.0), 2 * M_PI);
//            projection.sphericalHarmonics = debugSH;
//        } else {
//            SphericalHarmonics debugSH;
//            debugSH.contribute(glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, 0.0, 0.0), 2 * M_PI);
//            debugSH.contribute(glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, 0.0, 1.0), 2 * M_PI);
//            projection.sphericalHarmonics = debugSH;
//        }

        projection.sphericalHarmonics.convolve();
        projection.sphericalHarmonics.normalize();

        return projection;
    }

    void DiffuseLightProbeGenerator::projectSurfelClustersOnProbe(DiffuseLightProbe& probe) {
        probe.surfelClusterProjectionGroupOffset = (uint32_t)mProbeData->mSurfelClusterProjections.size();

        // FIXME: DEBUG
//        const SurfelCluster &cluster = mSurfelData->surfelClusters()[0];
//        SurfelClusterProjection projection = projectSurfelCluster(cluster, probe);
//        projection.surfelClusterIndex = (uint32_t)0;
//        mProbeData->mSurfelClusterProjections.push_back(projection);
//        probe.surfelClusterProjectionGroupSize++;

        for (size_t i = 0; i < mSurfelData->surfelClusters().size(); i++) {
            const SurfelCluster &cluster = mSurfelData->surfelClusters()[i];
            SurfelClusterProjection projection = projectSurfelCluster(cluster, probe);

            // Only accept projections with non-zero SH
            if (projection.sphericalHarmonics.magnitude2() > 0.0) {
                projection.surfelClusterIndex = (uint32_t)i;
                mProbeData->mSurfelClusterProjections.push_back(projection);
                probe.surfelClusterProjectionGroupSize++;
            }
        }
    }

#pragma mark - Public interface

    std::shared_ptr<DiffuseLightProbeData> DiffuseLightProbeGenerator::generateProbes(const Scene *scene, std::shared_ptr<const SurfelData> surfelData) {
        mProbeData = std::make_shared<DiffuseLightProbeData>();
        mSurfelData = surfelData;
        mScene = scene;

        AxisAlignedBox3D bb = scene->lightBakingVolume();
        glm::vec3 bbLengths = bb.max - bb.min;
        glm::vec3 resolution = glm::round(bbLengths / scene->difuseProbesSpacing());
        glm::vec3 step = bbLengths / (resolution - 1.0f);

        printf("Building grid probes...\n");
        Measurement::ExecutionTime("Grid probes placement took", [&]() {
            for (float z = bb.min.z; z <= bb.max.z + step.z / 2.0; z += step.z) {
                for (float y = bb.min.y; y <= bb.max.y + step.y / 2.0; y += step.y) {
                    for (float x = bb.min.x; x <= bb.max.x + step.x / 2.0; x += step.x) {
                        DiffuseLightProbe probe({ x, y, z });
                        projectSurfelClustersOnProbe(probe);
                        mProbeData->mProbes.push_back(probe);
                    }
                }
            }
            printf("Built %lu probes | %lu projections \n", mProbeData->mProbes.size(), mProbeData->mSurfelClusterProjections.size());
        });

        mProbeData->mGridResolution = resolution;
        mProbeData->initializeBuffers();

        return mProbeData;
    }

}
