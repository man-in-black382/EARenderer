//
//  OcclusionMapBuilder.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 09.05.2018.
//  Copyright © 2018 MPO. All rights reserved.
//

#include "OcclusionMapBuilder.hpp"
#include "GLCubemapSampler.hpp"

#include <vector>

namespace EARenderer {

#pragma mark - Lifecycle

    OcclusionMapBuilder::OcclusionMapBuilder(const Size2D& mapResolution)
    :
    mMapsResolution(mapResolution)
    { }

#pragma mark - Occlusion maps generation
#pragma mark - Private helpers

    void OcclusionMapBuilder::findOcclusionsDistancesForProbe(size_t probeIndex, Scene *scene) {

        auto& probe = scene->diffuseLightProbes()[probeIndex];

        for (int32_t cubeFaceIndex = 0; cubeFaceIndex < 6; cubeFaceIndex++) {

            GLCubemapFace face = CubemapFaceFromIndex(cubeFaceIndex);

            for (int32_t y = 0; y < mMapsResolution.height; y++) {

                for (int32_t x = 0; x < mMapsResolution.width; x++) {

                    glm::vec3 direction;
                    GLCubemapSampler::ComputeSampleVector(face, x, y, mMapsResolution, direction);

                    float hitDistance = std::numeric_limits<float>::max();
                    Ray3D ray(probe.position, direction);

                    if (scene->rayTracer()->rayHit(ray, hitDistance)) {
                        printf("Texture UVs: %d %d \n", x, y);
                        printf("Sample vector: %f %f %f\n", direction.x, direction.y, direction.z);
                        printf("Hit distance: %f \n\n", hitDistance);

                        int32_t flatIndex = y * mMapsResolution.width + x;
                    }
                }
            }
        }
        
    }

#pragma mark - Public interface

    void OcclusionMapBuilder::buildLightProbeOcclusionMaps(Scene *scene) {
        size_t probeCount = scene->diffuseLightProbes().size();
        size_t occlusionMapFaceCount = 6;
        size_t texelCount = mMapsResolution.width * mMapsResolution.height * probeCount * occlusionMapFaceCount;

        mOcclusionDistances.assign(texelCount, 0);

        for (size_t i = 0; i < scene->diffuseLightProbes().size(); i++) {
            findOcclusionsDistancesForProbe(i, scene);
        }
    }

}
