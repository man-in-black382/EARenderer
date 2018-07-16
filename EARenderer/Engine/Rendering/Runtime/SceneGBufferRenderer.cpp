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
        mFramebuffer.attachTexture(mGBuffer->linearDepth, GLFramebuffer::ColorAttachment::Attachment1);
        mFramebuffer.attachTexture(mGBuffer->hyperbolicDepth);

        // Preallocate HiZ buffer mipmaps
        mGBuffer->linearDepth.generateMipmaps();
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
            mHiZBufferShader.setTexture(mGBuffer->linearDepth);
        });

        size_t unnecessaryHighestMipsCount = 3; // We're stopping at 8x8 mip level, which means we don't need 3 highest mips
        for (size_t mipLevel = 0; mipLevel < mGBuffer->linearDepth.mipMapsCount() - unnecessaryHighestMipsCount; mipLevel++) {
            mHiZBufferShader.setMipLevel(mipLevel);
            mFramebuffer.attachTexture(mGBuffer->linearDepth, GLFramebuffer::ColorAttachment::Attachment0, mipLevel + 1);
            GLViewport(mGBuffer->linearDepth.mipMapSize(mipLevel + 1)).apply();
            mFramebuffer.clear(GLFramebuffer::UnderlyingBuffer::Color | GLFramebuffer::UnderlyingBuffer::Depth);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        }
    }

#pragma mark - Public Interface

    void SceneGBufferRenderer::render() {
        generateGBuffer();
//        generateHiZBuffer();
    }

}
