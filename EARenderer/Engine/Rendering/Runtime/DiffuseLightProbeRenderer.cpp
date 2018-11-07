//
//  DiffuseLightProbeRenderer.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 01.10.2018.
//  Copyright © 2018 MPO. All rights reserved.
//

#include "DiffuseLightProbeRenderer.hpp"
#include "Drawable.hpp"

namespace EARenderer {

#pragma mark - Lifecycle

    DiffuseLightProbeRenderer::DiffuseLightProbeRenderer(const Scene *scene,
                                                         std::shared_ptr<const DiffuseLightProbeData> probeData,
                                                         std::shared_ptr<const std::array<GLLDRTexture3D, 4>> sphericalHarmonics)
    :
    mScene(scene),
    mProbeData(probeData),
    mSphericalHarmonics(sphericalHarmonics)
    {
        mDiffuseProbesVAO.initialize(probeData->probes(), {
            GLVertexAttribute::UniqueAttribute(sizeof(glm::vec3), glm::vec3::length())
        });
    }

#pragma mark - Setters

    void DiffuseLightProbeRenderer::setRenderingSettings(const RenderingSettings& settings) {
        mRenderingSettings = settings;
    }

#pragma mark - Rendering

    void DiffuseLightProbeRenderer::render() {
        mDiffuseProbesVAO.bind();
        mGridProbeRenderingShader.bind();
        mGridProbeRenderingShader.setCamera(*mScene->camera());
        mGridProbeRenderingShader.setSphereRadius(mRenderingSettings.probeSettings.sphereRadius);
        mGridProbeRenderingShader.setWorldBoundingBox(mScene->lightBakingVolume());
        mGridProbeRenderingShader.setProbesGridResolution(mProbeData->gridResolution());
        mGridProbeRenderingShader.ensureSamplerValidity([&] {
            mGridProbeRenderingShader.setGridProbesSHTextures(*mSphericalHarmonics);
        });

        Drawable::Point::Draw(mProbeData->probes().size());
    }

}
