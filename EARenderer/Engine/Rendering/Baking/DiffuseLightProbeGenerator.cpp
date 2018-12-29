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

    float DiffuseLightProbeGenerator::surfelSolidAngle(const Surfel &surfel, const DiffuseLightProbe &probe) {
        glm::vec3 Wps = surfel.position - probe.position;
        float distance2 = glm::length2(Wps);
        Wps = glm::normalize(Wps);

        float distanceTerm = std::min(surfel.area / distance2, 1.f);

        float visibilityTerm = std::max(glm::dot(-surfel.normal, Wps), 0.f);

        float visibilityTest = 0.0;

        // Save ray casts if surfel's facing away from the standpoint
        if (visibilityTerm > 0.0) {
            constexpr float p0Offset = 0.01; // Offset line segment points to avoid erroneous collision detections at surfel positions,
            constexpr float p1Offset = 0.01; // which will happen a lot since the're located exactly on the surface of geometry
            visibilityTest = mScene->rayTracer()->lineSegmentOccluded(probe.position, surfel.position, p0Offset, p1Offset) ? 0.0f : 1.0f;
        }

        return distanceTerm * visibilityTerm * visibilityTest;
    }

    SurfelClusterProjection DiffuseLightProbeGenerator::projectSurfelCluster(const SurfelCluster &cluster, const DiffuseLightProbe &probe) {
        SurfelClusterProjection projection;

        for (size_t i = cluster.surfelOffset; i < cluster.surfelOffset + cluster.surfelCount; i++) {
            const Surfel &surfel = mSurfelData->surfels()[i];
            glm::vec3 Wps_norm = glm::normalize(surfel.position - probe.position);
            float solidAngle = surfelSolidAngle(surfel, probe);

            if (solidAngle > 0.0) {
                // Accumulating in YCoCg space to enable compression possibilities
                auto ycocg = surfel.albedo.convertedTo(Color::Space::YCoCg).rgb();
                projection.sphericalHarmonics.contribute(Wps_norm, ycocg, solidAngle);
            }
        }

        projection.sphericalHarmonics.convolve();
        projection.sphericalHarmonics.scale(glm::vec3(1.0f / (4.0f * M_PI)));

        return projection;
    }

    void DiffuseLightProbeGenerator::projectSurfelClustersOnProbe(DiffuseLightProbe &probe) {
        probe.surfelClusterProjectionGroupOffset = (uint32_t) mProbeData->mSurfelClusterProjections.size();

        for (size_t i = 0; i < mSurfelData->surfelClusters().size(); i++) {
            const SurfelCluster &cluster = mSurfelData->surfelClusters()[i];
            SurfelClusterProjection projection = projectSurfelCluster(cluster, probe);

            // Only accept projections with non-zero SH
            if (projection.sphericalHarmonics.magnitude() > 10e-7) {
                projection.surfelClusterIndex = (uint32_t) i;
                mProbeData->mSurfelClusterProjections.push_back(projection);
                probe.surfelClusterProjectionGroupSize++;
            }
        }
    }

    void DiffuseLightProbeGenerator::projectSkyOnProbe(DiffuseLightProbe &probe) {

        float sampleDelta = 0.025;
        int32_t iterationCount = 0;

        // Phi - azimuth (horizontal) angle
        for (float phi = 0.0; phi < M_PI * 2.0; phi += sampleDelta) {

            float sinPhi = sin(phi);
            float cosPhi = cos(phi);

            // Theta - zenith (vertical) angle
            for (float theta = 0.0; theta < M_PI; theta += sampleDelta) {

                float sinTheta = sin(theta);
                float cosTheta = cos(theta);

                glm::vec3 sampleVector(sinTheta * cosPhi, sinTheta * sinPhi, cosTheta);

                float distance = 0;
                Ray3D sampleRay(probe.position, sampleVector);

                // If sky is visible (not obstructed by geometry) contribute to the spherical harmonics in that direction
                if (!mScene->rayTracer()->rayHit(sampleRay, distance)) {
                    // Scale by sin(theta) to account for the smaller sample areas in the higher hemisphere areas
                    probe.skySphericalHarmonics.contribute(sampleVector, glm::vec3(1.0), sinTheta);
                }

                iterationCount++;
            }
        }

        probe.skySphericalHarmonics.scale(glm::vec3(1.0 / iterationCount));
        probe.skySphericalHarmonics.convolve();
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
                        DiffuseLightProbe probe({x, y, z});
                        projectSurfelClustersOnProbe(probe);
                        projectSkyOnProbe(probe);
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
