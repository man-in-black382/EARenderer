//
//  IndirectLightAccumulator.cpp
//  EARenderer
//
//  Created by Pavel Muratov on 9/5/18.
//  Copyright © 2018 MPO. All rights reserved.
//

#include "IndirectLightAccumulator.hpp"

namespace EARenderer {

#pragma mark - Lifecycle

    IndirectLightAccumulator::IndirectLightAccumulator(const Scene *scene,
                                                       std::shared_ptr<const SurfelData> surfelData,
                                                       std::shared_ptr<const DiffuseLightProbeData> probeData,
                                                       std::shared_ptr<const ShadowMapper> shadowMapper)
    :
    mScene(scene),
    mSurfelData(surfelData),
    mProbeData(probeData),
    mShadowMapper(shadowMapper),
    mFramebuffer(framebufferResolution()),
    mGridProbeSHMaps(gridProbeSHMaps()),
    mSurfelsLuminanceMap(std::make_shared<GLFloatTexture2D<GLTexture::Float::R16F>>(surfelData->surfelsGBuffer()->size(), nullptr, GLTexture::Filter::None)),
    mSurfelClustersLuminanceMap(std::make_shared<GLFloatTexture2D<GLTexture::Float::R16F>>(surfelData->surfelClustersGBuffer()->size(), nullptr, GLTexture::Filter::None))
    { }

    Size2D IndirectLightAccumulator::framebufferResolution() {
        Size2D probeGridResolution(mProbeData->gridResolution().x, mProbeData->gridResolution().y);
        Size2D surfelLuminanceMapResolution(mSurfelData->surfelsGBuffer()->size());
        Size2D clusterLuminanceMapResolution(mSurfelData->surfelClustersGBuffer()->size());
        return probeGridResolution.makeUnion(surfelLuminanceMapResolution).makeUnion(clusterLuminanceMapResolution);
    }

    std::shared_ptr<std::array<GLLDRTexture3D, 4>> IndirectLightAccumulator::gridProbeSHMaps() {
        auto resolution = mProbeData->gridResolution();
        return std::shared_ptr<std::array<GLLDRTexture3D, 4>>(new std::array<GLLDRTexture3D, 4> {
            GLLDRTexture3D(Size2D(resolution.x, resolution.y), resolution.z),
            GLLDRTexture3D(Size2D(resolution.x, resolution.y), resolution.z),
            GLLDRTexture3D(Size2D(resolution.x, resolution.y), resolution.z),
            GLLDRTexture3D(Size2D(resolution.x, resolution.y), resolution.z)
        });
    }

#pragma mark - Getters / Setters

    void IndirectLightAccumulator::setRenderingSettings(const RenderingSettings& settings) {
        mSettings = settings;
    }

    std::shared_ptr<std::array<GLLDRTexture3D, 4>> IndirectLightAccumulator::gridProbesSphericalHarmonics() const {
        return mGridProbeSHMaps;
    }

    std::shared_ptr<GLFloatTexture2D<GLTexture::Float::R16F>> IndirectLightAccumulator::surfelsLuminanceMap() const {
        return mSurfelsLuminanceMap;
    }

    std::shared_ptr<GLFloatTexture2D<GLTexture::Float::R16F>> IndirectLightAccumulator::surfelClustersLuminanceMap() const {
        return mSurfelClustersLuminanceMap;
    }

#pragma mark - Private Helpers

    void IndirectLightAccumulator::relightSurfels() {
        const DirectionalLight& directionalLight = mScene->directionalLight();

        mFramebuffer.redirectRenderingToTextures(GLViewport(mSurfelsLuminanceMap->size()), mSurfelsLuminanceMap);

        mSurfelLightingShader.bind();
        mSurfelLightingShader.setLight(directionalLight);
        mSurfelLightingShader.setSettings(mSettings);
        mSurfelLightingShader.setShadowCascades(mShadowMapper->cascades());
        mSurfelLightingShader.setWorldBoundingBox(mScene->lightBakingVolume());

        mSurfelLightingShader.ensureSamplerValidity([&]() {
            mSurfelLightingShader.setExponentialShadowMap(*mShadowMapper->directionalShadowMap());
            mSurfelLightingShader.setSurfelsGBuffer(*mSurfelData->surfelsGBuffer());
            mSurfelLightingShader.setGridProbesSHTextures(*mGridProbeSHMaps);
            mSurfelLightingShader.setProbePositions(*mProbeData->probePositionsBufferTexture());
        });

        TriangleStripQuad::Draw();
    }

    void IndirectLightAccumulator::averageSurfelClusterLuminances() {
        mSurfelClusterAveragingShader.bind();
        mFramebuffer.redirectRenderingToTextures(GLViewport(mSurfelClustersLuminanceMap->size()), mSurfelClustersLuminanceMap);

        mSurfelClusterAveragingShader.ensureSamplerValidity([&]() {
            mSurfelClusterAveragingShader.setSurfelClustersGBuffer(*mSurfelData->surfelClustersGBuffer());
            mSurfelClusterAveragingShader.setSurfelsLuminaceMap(*mSurfelsLuminanceMap);
        });

        TriangleStripQuad::Draw();
    }

    void IndirectLightAccumulator::updateGridProbes() {
        GLViewport viewport(Size2D(mProbeData->gridResolution().x, mProbeData->gridResolution().y));
        mFramebuffer.redirectRenderingToTextures(viewport, &(*mGridProbeSHMaps)[0], &(*mGridProbeSHMaps)[1], &(*mGridProbeSHMaps)[2], &(*mGridProbeSHMaps)[3]);

        mGridProbesUpdateShader.bind();
        mGridProbesUpdateShader.ensureSamplerValidity([&] {
            mGridProbesUpdateShader.setProbeProjectionsMetadata(*mProbeData->probeClusterProjectionsMetadataBufferTexture());
            mGridProbesUpdateShader.setProjectionClusterIndices(*mProbeData->projectionClusterIndicesBufferTexture());
            mGridProbesUpdateShader.setProjectionClusterSphericalHarmonics(*mProbeData->projectionClusterSHsBufferTexture());
            mGridProbesUpdateShader.setSurfelClustersLuminaceMap(*mSurfelClustersLuminanceMap);
            mGridProbesUpdateShader.setProbesGridResolution(mProbeData->gridResolution());
        });

        TriangleStripQuad::Draw(mProbeData->gridResolution().z);
    }

#pragma mark - Public Interface

    void IndirectLightAccumulator::render() {
        relightSurfels();
        averageSurfelClusterLuminances();
        updateGridProbes();
    }

}