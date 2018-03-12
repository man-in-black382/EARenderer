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

namespace EARenderer {

#pragma mark - Lifecycle

    SurfelRenderer::SurfelRenderer(Scene* scene, ResourcePool* resourcePool)
    :
    mScene(scene),
    mResourcePool(resourcePool)
    {
        for (auto& cluster : scene->surfelClusters()) {
            mSurfelClusterVAOs.emplace_back();
            mSurfelClusterVAOs.back().initialize(scene->surfels().data() + cluster.surfelOffset, cluster.surfelCount, {
                GLVertexAttribute::UniqueAttribute(sizeof(glm::vec3), glm::vec3::length()),
                GLVertexAttribute::UniqueAttribute(sizeof(glm::vec3), glm::vec3::length()),
                GLVertexAttribute::UniqueAttribute(sizeof(glm::vec3), glm::vec3::length()),
                GLVertexAttribute::UniqueAttribute(sizeof(glm::vec2), glm::vec2::length()),
                GLVertexAttribute::UniqueAttribute(sizeof(float), 1)
            });
        }
    }

#pragma mark - Public interface

    void SurfelRenderer::render() {
        mSurfelRenderingShader.bind();

        auto vp = mScene->camera()->viewProjectionMatrix();
        mSurfelRenderingShader.setViewProjectionMatrix(vp);
        mSurfelRenderingShader.setSurfelRadius(0.0375);

        for (size_t i = 0; i < mSurfelClusterVAOs.size(); i++) {
            mSurfelClusterVAOs[i].bind();
            glDrawArrays(GL_POINTS, 0, static_cast<GLint>(mScene->surfelClusters()[i].surfelCount));
        }
    }
    
}
