//
//  SurfelsRenderer.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 24.12.2017.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "SurfelRenderer.hpp"
#include "SurfelGenerator.hpp"
#include "WavefrontMeshLoader.hpp"
#include "FileManager.hpp"
#include "GLVertexAttribute.hpp"

#include <glm/gtx/transform.hpp>

#include <random>

namespace EARenderer {

#pragma mark - Lifecycle

    SurfelRenderer::SurfelRenderer(Scene* scene, ResourcePool* resourcePool)
    :
    mScene(scene),
    mResourcePool(resourcePool)
    {
        std::random_device device;
        std::mt19937 engine(device());
        std::uniform_real_distribution<float> distribution(0.3, 1.0);

        for (auto& cluster : scene->surfelClusters()) {
            mSurfelClusterVAOs.emplace_back();
            mSurfelClusterVAOs.back().initialize(scene->surfels().data() + cluster.surfelOffset, cluster.surfelCount, {
                GLVertexAttribute::UniqueAttribute(sizeof(glm::vec3), glm::vec3::length()),
                GLVertexAttribute::UniqueAttribute(sizeof(glm::vec3), glm::vec3::length()),
                GLVertexAttribute::UniqueAttribute(sizeof(glm::vec3), glm::vec3::length()),
                GLVertexAttribute::UniqueAttribute(sizeof(glm::vec2), glm::vec2::length()),
                GLVertexAttribute::UniqueAttribute(sizeof(float), 1)
            });

            mSurfelClusterColors.emplace_back(Color(distribution(engine), distribution(engine), distribution(engine)));
        }
    }

#pragma mark - Public interface

    void SurfelRenderer::render(Mode renderingMode, float surfelRadius, size_t probeIndex) {
        mSurfelRenderingShader.bind();

        switch (renderingMode) {
            case Mode::Default: mSurfelRenderingShader.setShouldUseExternalColor(false); break;
            case Mode::Clusters: mSurfelRenderingShader.setShouldUseExternalColor(true); break;
        }

        auto vp = mScene->camera()->viewProjectionMatrix();
        mSurfelRenderingShader.setViewProjectionMatrix(vp);
        mSurfelRenderingShader.setSurfelRadius(surfelRadius);

        if (probeIndex != -1) {
            DiffuseLightProbe& probe = mScene->diffuseLightProbes()[probeIndex];
            size_t projectionGroupCount = probe.surfelClusterProjectionGroupCount;
            size_t projectionGroupOffset = probe.surfelClusterProjectionGroupOffset;

            for (size_t i = projectionGroupOffset; i < projectionGroupOffset + projectionGroupCount; i++) {
                SurfelClusterProjection& projection = mScene->surfelClusterProjections()[i];
                mSurfelClusterVAOs[projection.surfelClusterIndex].bind();
                mSurfelRenderingShader.setExternalColor(mSurfelClusterColors[projection.surfelClusterIndex]);
                glDrawArrays(GL_POINTS, 0, static_cast<GLint>(mScene->surfelClusters()[projection.surfelClusterIndex].surfelCount));
            }
        } else {
            for (size_t i = 0; i < mSurfelClusterVAOs.size(); i++) {
                mSurfelClusterVAOs[i].bind();
                mSurfelRenderingShader.setExternalColor(mSurfelClusterColors[i]);
                glDrawArrays(GL_POINTS, 0, static_cast<GLint>(mScene->surfelClusters()[i].surfelCount));
            }
        }
    }
    
}
