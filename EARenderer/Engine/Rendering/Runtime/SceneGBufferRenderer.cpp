//
//  SceneGBufferRenderer.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 30.06.2018.
//  Copyright © 2018 MPO. All rights reserved.
//

#include "SceneGBufferRenderer.hpp"

namespace EARenderer {

#pragma mark - Lifecycle

    SceneGBufferRenderer::SceneGBufferRenderer(const Scene* scene, const RenderingSettings& settings)
    :
    mScene(scene),
    mFramebuffer(settings.resolution),
    mDepthRenderbuffer(settings.resolution),
    mGBuffer(std::make_shared<GLHDRTexture2DArray>(settings.resolution, 3, GLTexture::Filter::None))
    {
        mFramebuffer.attachTextureLayer(*mGBuffer, 0, GLFramebuffer::ColorAttachment::Attachment0);
        mFramebuffer.attachTextureLayer(*mGBuffer, 1, GLFramebuffer::ColorAttachment::Attachment1);
        mFramebuffer.attachTextureLayer(*mGBuffer, 2, GLFramebuffer::ColorAttachment::Attachment2);
        mFramebuffer.attachRenderbuffer(mDepthRenderbuffer);
    }

#pragma mark - Getters

    std::shared_ptr<const GLHDRTexture2DArray> SceneGBufferRenderer::GBuffer() const {
        return mGBuffer;
    }

#pragma mark - Rendering

    void SceneGBufferRenderer::render() {
        mFramebuffer.bind();
        mFramebuffer.viewport().apply();

        mGBufferShader.bind();
        mGBufferShader.setCamera(*(mScene->camera()));

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        for (ID instanceID : mScene->meshInstances()) {
            auto& instance = mScene->meshInstances()[instanceID];
            auto& subMeshes = ResourcePool::shared().meshes[instance.meshID()].subMeshes();

            mGBufferShader.setModelMatrix(instance.transformation().modelMatrix());

            for (ID subMeshID : subMeshes) {
                auto& subMesh = subMeshes[subMeshID];
                auto& material = ResourcePool::shared().materials[instance.materialIDForSubMeshID(subMeshID)];

                mGBufferShader.ensureSamplerValidity([this, &material]() {
                    mGBufferShader.setMaterial(material);
                });

                subMesh.draw();
            }
        }
    }

}
