//
//  BoxRenderer.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 08.03.2018.
//  Copyright © 2018 MPO. All rights reserved.
//

#include "BoxRenderer.hpp"

namespace EARenderer {

#pragma mark - Lifecycle

    BoxRenderer::BoxRenderer(Scene *scene)
    :
    mBoxSidesRenderingShader(GLSLCubeRendering::Mode::Sides),
    mBoxEdgesRenderingShader(GLSLCubeRendering::Mode::Edges),
    mScene(scene)
    {
        for (ID meshInstanceID : mScene->staticMeshInstanceIDs()) {
            auto& instance = mScene->meshInstances()[meshInstanceID];
            auto& mesh = ResourcePool::shared().meshes[instance.meshID()];
            auto& subMeshes = mesh.subMeshes();

            for (ID subMeshID : subMeshes) {
                auto& subMesh = subMeshes[subMeshID];

                auto box = subMesh.boundingBox().transformedBy(instance.transformation());
                mPoints.push_back(box.min);
                mPoints.push_back(box.max);
            }
        }

        mVAO.initialize(mPoints, { GLVertexAttribute::UniqueAttribute(sizeof(glm::vec3), glm::vec3::length()) });
    }

    BoxRenderer::BoxRenderer(Scene *scene, const std::vector<AxisAlignedBox3D>& boxes)
    :
    mBoxSidesRenderingShader(GLSLCubeRendering::Mode::Sides),
    mBoxEdgesRenderingShader(GLSLCubeRendering::Mode::Edges),
    mScene(scene)
    {
        for (auto& box : boxes) {
            mPoints.push_back(box.min);
            mPoints.push_back(box.max);
        }

        mVAO.initialize(mPoints, { GLVertexAttribute::UniqueAttribute(sizeof(glm::vec3), glm::vec3::length()) });
    }

#pragma mark - Rendering

    void BoxRenderer::render() {
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_BLEND);
        glDisable(GL_CULL_FACE);

        mVAO.bind();

        mBoxEdgesRenderingShader.bind();
        mBoxEdgesRenderingShader.setViewProjectionMatrix(mScene->camera()->viewProjectionMatrix());
        mBoxEdgesRenderingShader.setColor(Color(1.0, 0.4, 0.7, 1.0));
        glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(mPoints.size()));

        mBoxSidesRenderingShader.bind();
        mBoxSidesRenderingShader.setViewProjectionMatrix(mScene->camera()->viewProjectionMatrix());
        mBoxSidesRenderingShader.setColor(Color(0.5, 0.6, 0.8, 0.4));
        glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(mPoints.size()));

        glDisable(GL_BLEND);
        glEnable(GL_CULL_FACE);
    }

}
