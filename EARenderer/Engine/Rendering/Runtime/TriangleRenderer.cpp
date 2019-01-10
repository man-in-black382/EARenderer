//
//  TriangleRenderer.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 30.12.2017.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "TriangleRenderer.hpp"
#include "Drawable.hpp"

namespace EARenderer {

#pragma mark - Lifecycle

    TriangleRenderer::TriangleRenderer(const Scene *scene, const SharedResourceStorage *resourceStorage, const GPUResourceController *gpuResourceController)
            : mScene(scene), mResourceStorage(resourceStorage), mGPUResourceController(gpuResourceController) {}

#pragma mark - Rendering

    void TriangleRenderer::render() {
        glEnable(GL_MULTISAMPLE);

        mTriangleRenderingShader.bind();
        mTriangleRenderingShader.setColor(Color::White());

        glm::mat4 viewProjection = mScene->camera()->viewProjectionMatrix();

        for (ID meshInstanceID : mScene->meshInstances()) {
            auto &instance = mScene->meshInstances()[meshInstanceID];
            auto &subMeshes = mResourceStorage->mesh(instance.meshID()).subMeshes();

            glm::mat4 mvp = viewProjection * instance.modelMatrix();
            mTriangleRenderingShader.setModelViewProjectionMatrix(mvp);

            for (ID subMeshID : subMeshes) {
                auto &subMesh = subMeshes[subMeshID];
                Drawable::TriangleMesh::Draw(mGPUResourceController->subMeshVBODataLocation(instance.meshID(), subMeshID));
            }
        }

        glDisable(GL_MULTISAMPLE);
    }

}
