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
        mSurfelsVAO.initialize(scene->surfels().data(), scene->surfels().size(), {
            GLVertexAttribute::UniqueAttribute(sizeof(glm::vec3), glm::vec3::length()),
            GLVertexAttribute::UniqueAttribute(sizeof(glm::vec3), glm::vec3::length()),
            GLVertexAttribute::UniqueAttribute(sizeof(glm::vec3), glm::vec3::length()),
            GLVertexAttribute::UniqueAttribute(sizeof(glm::vec2), glm::vec2::length()),
            GLVertexAttribute::UniqueAttribute(sizeof(float), 1)
        });

        glEnable(GL_PROGRAM_POINT_SIZE);
    }

#pragma mark - Public interface

    void SurfelRenderer::render() {
        mSurfelsVAO.bind();
        mSurfelRenderingShader.bind();

        auto vp = mScene->camera()->viewProjectionMatrix();
        mSurfelRenderingShader.setViewProjectionMatrix(vp);
        mSurfelRenderingShader.setSurfelRadius(0.1);

        glDrawArrays(GL_POINTS, 0, static_cast<GLint>(mScene->surfels().size()));
    }
    
}
