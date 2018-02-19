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

namespace EARenderer {

#pragma mark - Lifecycle

    SurfelRenderer::SurfelRenderer(Scene* scene, ResourcePool* resourcePool)
    :
    mScene(scene),
    mResourcePool(resourcePool),
    mSphere(FileManager::shared().resourceRootPath() + "_engine_resource_sphere.obj")
    {
        mVAO.initialize(scene->surfels().data(), scene->surfels().size(), {
            GLVertexAttribute::UniqueAttribute(sizeof(glm::vec3), glm::vec3::length()),
            GLVertexAttribute::UniqueAttribute(sizeof(glm::vec3), glm::vec3::length())
        });

        glEnable(GL_PROGRAM_POINT_SIZE);
    }

#pragma mark - Private heplers

    void SurfelRenderer::appendTransformationBufferToSphere() {
        
    }

#pragma mark - Public interface

    void SurfelRenderer::render() {
        mVAO.bind();
        mSurfelRenderingShader.bind();

        auto mvp = mScene->camera()->viewProjectionMatrix();
        mSurfelRenderingShader.setModelViewProjectionMatrix(mvp);
        mSurfelRenderingShader.setColor(Color(0.2, 1.0, 0.3, 1.0));

        glDrawArrays(GL_POINTS, 0, static_cast<GLint>(mScene->surfels().size()));
    }
    
}
