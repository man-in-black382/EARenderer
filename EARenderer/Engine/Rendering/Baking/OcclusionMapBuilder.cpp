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
    mOcclusionMapFaceResolution(mapResolution)
    { }

#pragma mark - Occlusion maps generation
#pragma mark - Private helpers

    void OcclusionMapBuilder::calculateTextureResolution() {
        size_t probeCount = mProbeData->probes().size();
        size_t occlusionMapFaceCount = 6;

        size_t texelCount = mOcclusionMapFaceResolution.width * mOcclusionMapFaceResolution.height * probeCount * occlusionMapFaceCount;

        Size2D estimatedSize = GLTexture::EstimatedSize(texelCount);

        size_t tail = (size_t)estimatedSize.width % size_t(occlusionMapFaceCount * mOcclusionMapFaceResolution.width);
        size_t alignedWidth = estimatedSize.width - tail;
        size_t alignedHeight = texelCount / alignedWidth;

        mTextureResolution = Size2D(alignedWidth, alignedHeight);
    }

    void OcclusionMapBuilder::findOcclusionsDistancesForProbe(int32_t probeIndex) {

        const int32_t kFaceCount = 6;

        auto& probe = mProbeData->probes()[probeIndex];
        int32_t probeShadowMapsPerRow = mTextureResolution.width / mOcclusionMapFaceResolution.width / 6;
        int32_t yOffset = probeIndex / probeShadowMapsPerRow;
        int32_t xOffset = probeIndex % probeShadowMapsPerRow;

        int32_t xOffsetInPixels = xOffset * mOcclusionMapFaceResolution.width * kFaceCount;

        printf("Probe idx: %d | X offset: %d | Y offset: %d \n", probeIndex, xOffset, yOffset);

        for (int32_t cubeFaceIndex = 0; cubeFaceIndex < kFaceCount; cubeFaceIndex++) {

            GLCubemapFace face = CubemapFaceFromIndex(cubeFaceIndex);

            for (int32_t localY = 0; localY < mOcclusionMapFaceResolution.height; localY++) {

                for (int32_t localX = 0; localX < mOcclusionMapFaceResolution.width; localX++) {

                    int32_t globalX = localX + mOcclusionMapFaceResolution.width * cubeFaceIndex + xOffsetInPixels;
                    int32_t globalY = localY + mOcclusionMapFaceResolution.height * yOffset;

                    glm::vec3 direction;
                    GLCubemapSampler::ComputeSampleVector(face, localX, localY, mOcclusionMapFaceResolution, direction);

                    float hitDistance = std::numeric_limits<float>::max();
                    Ray3D ray(probe.position, direction);

                    if (mScene->rayTracer()->rayHit(ray, hitDistance)) {
                        printf("Texture coords: %d %d \n", globalX, globalY);

                        int32_t flatIndex = globalY * mTextureResolution.width + globalX;
                        mOcclusionDistances[flatIndex] = hitDistance;
                    }
                }
            }
        }
        
    }

#pragma mark - Public interface

    std::shared_ptr<GLHDRTexture2D> OcclusionMapBuilder::buildLightProbeOcclusionMaps(const Scene *scene, std::shared_ptr<const DiffuseLightProbeData> probeData) {
        mScene = scene;
        mProbeData = probeData;

        calculateTextureResolution();
        mOcclusionDistances.assign(mTextureResolution.width * mTextureResolution.height, 0);

        for (int32_t i = 0; i < mProbeData->probes().size(); i++) {
            findOcclusionsDistancesForProbe(i);
        }

        return std::shared_ptr<GLHDRTexture2D>(new GLHDRTexture2D(mOcclusionDistances, mTextureResolution));
    }

}
