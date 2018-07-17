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
        mFramebuffer.attachTexture(mGBuffer->albedoRoughnessMetalnessAONormal, GLFramebuffer::ColorAttachment::Attachment0);
        mFramebuffer.attachTexture(mGBuffer->linearDepthHZB, GLFramebuffer::ColorAttachment::Attachment1);
        mFramebuffer.attachRenderbuffer(mDepthRenderbuffer);

        // Preallocate HiZ buffer mipmaps
        mGBuffer->linearDepthHZB.generateMipmaps();
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
        mFramebuffer.attachTexture(mGBuffer->linearDepthHZB, GLFramebuffer::ColorAttachment::Attachment1, 0);
        mFramebuffer.activateDrawBuffers(mGBuffer->albedoRoughnessMetalnessAONormal, mGBuffer->linearDepthHZB);
        mFramebuffer.clear(GLFramebuffer::UnderlyingBuffer::Color | GLFramebuffer::UnderlyingBuffer::Depth);

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
        mFramebuffer.bind();

        mHiZBufferShader.bind();
        mHiZBufferShader.ensureSamplerValidity([&]() {
            mHiZBufferShader.setTexture(mGBuffer->linearDepthHZB);
        });

        size_t unnecessaryHighestMipsCount = 3; // We're stopping at 8x8 mip level, which means we don't need 3 highest mips
        mGBuffer->HiZBufferMipCount = mGBuffer->linearDepthHZB.mipMapsCount() - unnecessaryHighestMipsCount;

        for (size_t mipLevel = 0; mipLevel < mGBuffer->HiZBufferMipCount; mipLevel++) {
            mHiZBufferShader.setMipLevel(mipLevel);

            Size2D mipSize = mGBuffer->linearDepthHZB.mipMapSize(mipLevel + 1);
            GLViewport(mipSize).apply();

            mFramebuffer.attachTexture(mGBuffer->linearDepthHZB, GLFramebuffer::ColorAttachment::Attachment1, mipLevel + 1);
            // Leave only linear depth attachment active so that other textures won't get corrupted
            mFramebuffer.activateDrawBuffers(mGBuffer->linearDepthHZB);
            mFramebuffer.clear(GLFramebuffer::UnderlyingBuffer::Color | GLFramebuffer::UnderlyingBuffer::Depth);

            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        }
    }

#pragma mark - Public Interface

    void SceneGBufferRenderer::render() {
        generateGBuffer();
        generateHiZBuffer();
    }

}
