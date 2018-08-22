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
    mGBuffer(std::make_shared<SceneGBuffer>(settings.resolution))
    {
        mFramebuffer.attachTexture(*mGBuffer->albedoRoughnessMetalnessAONormal, GLFramebuffer::ColorAttachment::Attachment0);
        mFramebuffer.attachTexture(*mGBuffer->HiZBuffer, GLFramebuffer::ColorAttachment::Attachment1);
        mFramebuffer.attachTexture(*mGBuffer->depthBuffer);

        // Preallocate HiZ buffer mipmaps
        mGBuffer->HiZBuffer->generateMipMaps();
    }

#pragma mark - Getters

    std::shared_ptr<const SceneGBuffer> SceneGBufferRenderer::GBuffer() const {
        return mGBuffer;
    }

#pragma mark - Rendering
#pragma mark - Private Helpers

    void SceneGBufferRenderer::generateGBuffer() {
        mFramebuffer.bind();
        mFramebuffer.viewport().apply();

        mGBufferShader.bind();
        mGBufferShader.setCamera(*(mScene->camera()));

        // Attach 0 mip again after HiZ buffer construction
        mFramebuffer.redirectRenderingToTexturesMip(0, mGBuffer->albedoRoughnessMetalnessAONormal, mGBuffer->HiZBuffer);

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

    void SceneGBufferRenderer::generateHiZBuffer() {
        // Disable depth writes to not pollute depth buffer with HIZ buffer quads
        glDepthMask(GL_FALSE);

        mFramebuffer.bind();

        mHiZBufferShader.bind();
        mHiZBufferShader.ensureSamplerValidity([&]() {
            mHiZBufferShader.setTexture(*mGBuffer->HiZBuffer);
        });

        mGBuffer->HiZBufferMipCount = mGBuffer->HiZBuffer->mipMapCount();

        for (size_t mipLevel = 0; mipLevel < mGBuffer->HiZBufferMipCount; mipLevel++) {
            mHiZBufferShader.setMipLevel(mipLevel);

            Size2D mipSize = mGBuffer->HiZBuffer->mipMapSize(mipLevel + 1);
            GLViewport(mipSize).apply();

            // Leave only linear depth attachment active so that other textures won't get corrupted
            mFramebuffer.redirectRenderingToTexturesMip(mipLevel + 1, mGBuffer->HiZBuffer);

            TriangleStripQuad::Draw();
        }

        glDepthMask(GL_TRUE);
    }

#pragma mark - Public Interface

    void SceneGBufferRenderer::render() {
        generateGBuffer();
        generateHiZBuffer();
    }

}
