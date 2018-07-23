//
//  DeferredSceneRenderer.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 30.06.2018.
//  Copyright © 2018 MPO. All rights reserved.
//

#include "DeferredSceneRenderer.hpp"
#include "GLShader.hpp"
#include "ResourcePool.hpp"
#include "Vertex1P4.hpp"
#include "Collision.hpp"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

namespace EARenderer {

#pragma mark - Lifecycle

    DeferredSceneRenderer::DeferredSceneRenderer(const Scene* scene,
                                                 const DefaultRenderComponentsProviding *provider,
                                                 const RenderingSettings& settings,
                                                 std::shared_ptr<const SurfelData> surfelData,
                                                 std::shared_ptr<const DiffuseLightProbeData> diffuseProbeData,
                                                 std::shared_ptr<const SceneGBuffer> GBuffer)
    :
    mScene(scene),
    mDefaultRenderComponentsProvider(provider),
    mSettings(settings),
    mSurfelData(surfelData),
    mDiffuseProbeData(diffuseProbeData),
    mGBuffer(GBuffer),
    mProbeGridResolution(scene->preferredProbeGridResolution()),

    // Shadow maps
    mDirectionalShadowTexturePool(std::make_shared<PostprocessTexturePool>(Size2D(1500))),
    mDirectionalExponentialShadowMap(mDirectionalShadowTexturePool->claim()),

    // Surfels and surfel clusters
    mSurfelsLuminanceMap(surfelData->surfelsGBuffer()->size(), nullptr, GLTexture::Filter::None),
    mSurfelClustersLuminanceMap(surfelData->surfelClustersGBuffer()->size(), nullptr, GLTexture::Filter::None),
    mSurfelsLuminanceFramebuffer(mSurfelsLuminanceMap.size()),
    mSurfelClustersLuminanceFramebuffer(mSurfelClustersLuminanceMap.size()),

    // Diffuse light probes
    mGridProbesSHMaps {
        GLLDRTexture3D(Size2D(mProbeGridResolution.x, mProbeGridResolution.y), mProbeGridResolution.z),
        GLLDRTexture3D(Size2D(mProbeGridResolution.x, mProbeGridResolution.y), mProbeGridResolution.z),
        GLLDRTexture3D(Size2D(mProbeGridResolution.x, mProbeGridResolution.y), mProbeGridResolution.z),
        GLLDRTexture3D(Size2D(mProbeGridResolution.x, mProbeGridResolution.y), mProbeGridResolution.z)
    },
    mGridProbesSHFramebuffer(Size2D(mProbeGridResolution.x, mProbeGridResolution.y)),

    // Effects
    mPostprocessTexturePool(std::make_shared<PostprocessTexturePool>(settings.resolution)),

    // Output frame
    mFrame(mPostprocessTexturePool->claim()),
    mPreviousFrame(mPostprocessTexturePool->claim()),
    mThresholdFilteredOutputFrame(mPostprocessTexturePool->claim())//,
//    mOutputDepthRenderbuffer(mOutputFrame->size()),
//    mOutputFramebuffer(mOutputFrame->size())
    {
        setupGLState();
        setupFramebuffers();
    }

#pragma mark - Setters

    void DeferredSceneRenderer::setRenderingSettings(const RenderingSettings& settings) {
        mSettings = settings;
    }

#pragma mark - Getters

    const FrustumCascades& DeferredSceneRenderer::shadowCascades() const {
        return mShadowCascades;
    }

#pragma mark - Initial setup

    void DeferredSceneRenderer::setupGLState() {
        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
        glDisable(GL_BLEND);
        glClearColor(0.0, 0.0, 0.0, 1.0);
        glClearDepth(1.0);
        glDepthFunc(GL_LEQUAL);
    }

    void DeferredSceneRenderer::setupFramebuffers() {
        mSurfelsLuminanceFramebuffer.attachTexture(mSurfelsLuminanceMap);
        mSurfelClustersLuminanceFramebuffer.attachTexture(mSurfelClustersLuminanceMap);

        mGridProbesSHFramebuffer.attachTexture(mGridProbesSHMaps[0], GLFramebuffer::ColorAttachment::Attachment0);
        mGridProbesSHFramebuffer.attachTexture(mGridProbesSHMaps[1], GLFramebuffer::ColorAttachment::Attachment1);
        mGridProbesSHFramebuffer.attachTexture(mGridProbesSHMaps[2], GLFramebuffer::ColorAttachment::Attachment2);
        mGridProbesSHFramebuffer.attachTexture(mGridProbesSHMaps[3], GLFramebuffer::ColorAttachment::Attachment3);
    }

#pragma mark - Rendering
#pragma mark - Runtime

    void DeferredSceneRenderer::bindDefaultFramebuffer() {
        if (mDefaultRenderComponentsProvider) {
            mDefaultRenderComponentsProvider->bindSystemFramebuffer();
            mDefaultRenderComponentsProvider->defaultViewport().apply();
        }
    }

    void DeferredSceneRenderer::swapFrames() {
        std::swap(mFrame, mPreviousFrame);
    }

    void DeferredSceneRenderer::performDepthPrepass() {
        bindDefaultFramebuffer();
        glClear(GL_DEPTH_BUFFER_BIT);

        mDepthPrepassShader.bind();
        mDepthPrepassShader.setCamera(*mScene->camera());

        for (ID instanceID : mScene->meshInstances()) {
            auto& instance = mScene->meshInstances()[instanceID];
            auto& subMeshes = ResourcePool::shared().meshes[instance.meshID()].subMeshes();

            mDepthPrepassShader.setModelMatrix(instance.transformation().modelMatrix());

            for (ID subMeshID : subMeshes) {
                auto& subMesh = subMeshes[subMeshID];
                subMesh.draw();
            }
        }
    }

    void DeferredSceneRenderer::renderExponentialShadowMapsForDirectionalLight() {
        auto renderTarget = mDirectionalShadowTexturePool->claim();
        mDirectionalShadowTexturePool->redirectRenderingToTextures(renderTarget);

        mDirectionalESMShader.bind();
        mDirectionalESMShader.setESMFactor(mSettings.meshSettings.ESMFactor);

        for (size_t cascade = 0; cascade < mShadowCascades.amount; cascade++) {
            // Ensure only one texture channel will be written to for each respective cascade
            glColorMask(cascade == 0, cascade == 1, cascade == 2, cascade == 3);

            mDirectionalESMShader.setLightMatrix(mShadowCascades.lightViewProjections[cascade]);

            for (ID meshInstanceID : mScene->meshInstances()) {
                auto& instance = mScene->meshInstances()[meshInstanceID];
                auto& subMeshes = ResourcePool::shared().meshes[instance.meshID()].subMeshes();

                mDirectionalESMShader.setModelMatrix(instance.transformation().modelMatrix());

                for (ID subMeshID : subMeshes) {
                    auto& subMesh = subMeshes[subMeshID];
                    subMesh.draw();
                }
            }
        }

        glColorMask(true, true, true, true);

        mShadowBlurEffect.blur(renderTarget, mDirectionalExponentialShadowMap,
                               mDirectionalShadowTexturePool, mSettings.meshSettings.shadowBlur);

        mDirectionalShadowTexturePool->putBack(renderTarget);
    }

    void DeferredSceneRenderer::relightSurfels() {
        const DirectionalLight& directionalLight = mScene->directionalLight();

        mSurfelsLuminanceFramebuffer.bind();
        mSurfelsLuminanceFramebuffer.viewport().apply();
        mSurfelsLuminanceFramebuffer.clear(GLFramebuffer::UnderlyingBuffer::Color | GLFramebuffer::UnderlyingBuffer::Depth);

        mSurfelLightingShader.bind();
        mSurfelLightingShader.setLight(directionalLight);
        mSurfelLightingShader.setMultibounceEnabled(mSettings.meshSettings.lightMultibounceEnabled);
        mSurfelLightingShader.ensureSamplerValidity([&]() {
            mSurfelLightingShader.setShadowCascades(mShadowCascades);
            mSurfelLightingShader.setExponentialShadowMap(*mDirectionalExponentialShadowMap);
            mSurfelLightingShader.setSurfelsGBuffer(*mSurfelData->surfelsGBuffer());
            mSurfelLightingShader.setGridProbesSHTextures(mGridProbesSHMaps);
            mSurfelLightingShader.setWorldBoundingBox(mScene->lightBakingVolume());
            mSurfelLightingShader.setProbePositions(*mDiffuseProbeData->probePositionsBufferTexture());
            mSurfelLightingShader.setSettings(mSettings);
        });

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }

    void DeferredSceneRenderer::averageSurfelClusterLuminances() {
        mSurfelClusterAveragingShader.bind();
        mSurfelClustersLuminanceFramebuffer.bind();
        mSurfelClustersLuminanceFramebuffer.viewport().apply();
        mSurfelClustersLuminanceFramebuffer.clear(GLFramebuffer::UnderlyingBuffer::Color | GLFramebuffer::UnderlyingBuffer::Depth);

        mSurfelClusterAveragingShader.ensureSamplerValidity([&]() {
            mSurfelClusterAveragingShader.setSurfelClustersGBuffer(*mSurfelData->surfelClustersGBuffer());
            mSurfelClusterAveragingShader.setSurfelsLuminaceMap(mSurfelsLuminanceMap);
        });

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }

    void DeferredSceneRenderer::updateGridProbes() {
        mGridProbesSHFramebuffer.bind();
        mGridProbesSHFramebuffer.viewport().apply();
        mGridProbesSHFramebuffer.clear(GLFramebuffer::UnderlyingBuffer::Color | GLFramebuffer::UnderlyingBuffer::Depth);

        mGridProbesUpdateShader.bind();
        mGridProbesUpdateShader.ensureSamplerValidity([&] {
            mGridProbesUpdateShader.setSurfelClustersLuminaceMap(mSurfelClustersLuminanceMap);
            mGridProbesUpdateShader.setProbeProjectionsMetadata(*mDiffuseProbeData->probeClusterProjectionsMetadataBufferTexture());
            mGridProbesUpdateShader.setProjectionClusterIndices(*mDiffuseProbeData->projectionClusterIndicesBufferTexture());
            mGridProbesUpdateShader.setProjectionClusterSphericalHarmonics(*mDiffuseProbeData->projectionClusterSHsBufferTexture());
            mGridProbesUpdateShader.setProbesGridResolution(mProbeGridResolution);
        });

        glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, (GLsizei)mProbeGridResolution.z);
    }

    void DeferredSceneRenderer::renderMeshes() {
        mCookTorranceShader.bind();

        mCookTorranceShader.setSettings(mSettings);
        mCookTorranceShader.setCamera(*(mScene->camera()));
        mCookTorranceShader.setLight(mScene->directionalLight());
        mCookTorranceShader.setWorldBoundingBox(mScene->lightBakingVolume());
        mCookTorranceShader.setProbePositions(*mDiffuseProbeData->probePositionsBufferTexture());
        mCookTorranceShader.setFrustumCascades(mShadowCascades);

        mCookTorranceShader.ensureSamplerValidity([&]() {
            mCookTorranceShader.setGBuffer(*mGBuffer);
            mCookTorranceShader.setExponentialShadowMap(*mDirectionalExponentialShadowMap);
            mCookTorranceShader.setGridProbesSHTextures(mGridProbesSHMaps);
        });

        mPostprocessTexturePool->redirectRenderingToTextures(mFrame, mThresholdFilteredOutputFrame);

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }

    void DeferredSceneRenderer::renderFinalImage(const GLFloatTexture2D<GLTexture::Float::RGBA16F>& image) {
        bindDefaultFramebuffer();
        glDisable(GL_DEPTH_TEST);

        mFSQuadShader.bind();
        mFSQuadShader.setApplyToneMapping(false);
        mFSQuadShader.ensureSamplerValidity([&]() {
            mFSQuadShader.setTexture(image);
        });

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glEnable(GL_DEPTH_TEST);
    }

#pragma mark - Public interface

    void DeferredSceneRenderer::render() {
        mShadowCascades = mScene->directionalLight().cascadesForWorldBoundingBox(mScene->boundingBox());

        swapFrames();

        renderExponentialShadowMapsForDirectionalLight();
        relightSurfels();
        averageSurfelClusterLuminances();
        updateGridProbes();
        renderMeshes();

//        auto ssrOutputTexture = mPostprocessTexturePool->claim();
//        mSSREffect.applyReflections(*mScene->camera(), mPreviousFrame, mGBuffer, ssrOutputTexture, mPostprocessTexturePool);

//        auto bloomOutputTexture = mPostprocessTexturePool->claim();
//        mBloomEffect.bloom(mFrame, mThresholdFilteredOutputFrame, bloomOutputTexture, mPostprocessTexturePool, mSettings.bloomSettings);
//
//        auto toneMappingOutputTexture = mPostprocessTexturePool->claim();
//        mToneMappingEffect.toneMap(bloomOutputTexture, toneMappingOutputTexture, mPostprocessTexturePool);

        renderFinalImage(*mFrame);

        debugSSR();

//        mPostprocessTexturePool->putBack(ssrOutputTexture);
//        mPostprocessTexturePool->putBack(bloomOutputTexture);
//        mPostprocessTexturePool->putBack(toneMappingOutputTexture);
    }

    // DEBUG

    glm::vec3 DeferredSceneRenderer::ReconstructWorldPosition(const Camera& camera, float depth, const glm::vec2& pixelCoords) {
        glm::vec2 vTexCoords = pixelCoords / glm::vec2(mSettings.resolution.width, mSettings.resolution.height);
        // Depth range in NDC is [-1; 1]
        // Default value for glDepthRange is [-1; 1]
        // OpenGL uses values from glDepthRange to transform depth to [0; 1] range during viewport transformation
        // To reconstruct world position using inverse camera matrices we need depth in [-1; 1] range again
        float z = depth * 2.0 - 1.0;
        glm::vec2 xy = vTexCoords * 2.0f - 1.0f;

        glm::vec4 clipSpacePosition = glm::vec4(xy, z, 1.0);
        glm::vec4 viewSpacePosition = camera.inverseProjectionMatrix() * clipSpacePosition;

        // Perspective division
        viewSpacePosition /= viewSpacePosition.w;

        glm::vec4 worldSpacePosition = camera.inverseViewMatrix() * viewSpacePosition;

        return worldSpacePosition;
    }

    glm::vec4 Decode8888(uint encoded) {
        glm::vec4 decoded;
        decoded.x = (0xFF000000u & encoded) >> 24;
        decoded.y = (0x00FF0000u & encoded) >> 16;
        decoded.z = (0x0000FF00u & encoded) >> 8;
        decoded.w = (0x000000FFu & encoded);
        decoded /= 255.0;
        return decoded;
    }

    glm::vec2 UnpackSnorm2x16(uint package, float range) {
        const float base = 32767.0;

        // Unpack encoded floats into individual variables
        uint uFirst = package >> 16;
        uint uSecond = package & 0x0000FFFFu;

        // Extract sign bits
        uint firstSignMask = uFirst & (1u << 15);
        uint secondSignMask = uSecond & (1u << 15);

        // If sign bit indicates negativity, fill MS 16 bits with 1s
        uFirst |= firstSignMask != 0 ? 0xFFFF0000u : 0x0u;
        uSecond |= secondSignMask != 0 ? 0xFFFF0000u : 0x0u;

        // Now get signed integer representation
        int iFirst = int(uFirst);
        int iSecond = int(uSecond);

        // At last, convert integers back to floats using range and base
        float fFirst = (float(iFirst) / base) * range;
        float fSecond = (float(iSecond) / base) * range;

        return glm::vec2(fFirst, fSecond);
    }

    DeferredSceneRenderer::GBuffer DeferredSceneRenderer::decodeGBuffer(const glm::uvec3& GBuffer) {
        struct GBuffer gBuffer;

        glm::vec4 albedoRoughness = Decode8888(GBuffer.x);
        uint metalnessAONormalZ = GBuffer.y;
        glm::vec2 metalnessAO = Decode8888(metalnessAONormalZ);
        float normalZ = UnpackSnorm2x16(metalnessAONormalZ, 1.0).y;
        glm::vec2 normalXY = UnpackSnorm2x16(GBuffer.z, 1.0);

        gBuffer.albedo    = albedoRoughness;
        gBuffer.normal    = glm::vec3(normalXY, normalZ);
        gBuffer.roughness = albedoRoughness.a;
        gBuffer.metalness = metalnessAO.r;
        gBuffer.AO        = metalnessAO.g;

        return gBuffer;
    }

    void DeferredSceneRenderer::StepThroughCell(glm::vec3& raySample, glm::vec3 rayDir, int mipLevel)
    {
        // Constant offset to make sure you cross to the next cell
        const float kCellStepOffset = 0.05;

        // Size of current mip
        Size2D mipSize = mGBuffer->HiZBuffer.mipMapSize(mipLevel);

        // UV converted to index in the mip
        glm::vec2 mipCellIndex = glm::vec2(raySample) * glm::vec2(mipSize.width, mipSize.height);

        // Find the cell boundary UV based on the direction of the ray
        // Take floor() or ceil() depending on the sign of rayDir.xy
        //
        glm::vec2 boundaryUV;
        boundaryUV.x = rayDir.x > 0 ? ceil(mipCellIndex.x) / float(mipSize.width) : floor(mipCellIndex.x) / float(mipSize.width);
        boundaryUV.y = rayDir.y > 0 ? ceil(mipCellIndex.y) / float(mipSize.height) : floor(mipCellIndex.y) / float(mipSize.height);

        // We can now represent the cell boundary as being formed by the intersection of
        // two lines which can be represented by
        //
        // x = boundaryUV.x
        // y = boundaryUV.y
        //
        // Intersect the parametric equation of the Ray with each of these lines
        //
        glm::vec2 t;
        t.x = (boundaryUV.x - raySample.x) / rayDir.x;
        t.y = (boundaryUV.y - raySample.y) / rayDir.y;

        // Pick the cell intersection that is closer, and march to that cell
        float axis = abs(t.x) < abs(t.y) ? t.x : t.y;
        raySample += (axis/* + kCellStepOffset*/) * rayDir;

        glm::vec2 newMipCellIndex = glm::vec2(raySample) * glm::vec2(mipSize.width, mipSize.height);
        printf("");
    }

    bool DeferredSceneRenderer::GetReflection(glm::vec3 worldReflectionVec,
                                              glm::vec3 screenSpaceReflectionVec,
                                              glm::vec3 screenSpacePos,
                                              glm::vec3& reflectionColor)
    {
        glm::vec3 raySample = screenSpacePos;
        float viewportAttenuationFactor = 0.0;
        int mipLevel = 0;

        int iterations = 0;

        while (mipLevel > -1 && mipLevel <= mGBuffer->HiZBufferMipCount) {
            // Cross a single texel in the HZB for the current mipLevel
            StepThroughCell(raySample, screenSpaceReflectionVec, mipLevel);

            glm::vec2 raySampleXY(raySample);

            auto HiZBufferSampler = mGBuffer->HiZBuffer.sampleTexels(mipLevel);
            float ZBufferValue = HiZBufferSampler.sample(raySampleXY);

            if (raySample.z < ZBufferValue) {
                // If we did not intersect, perform successive test on the next
                // higher mip level (and thus take larger steps)
                mipLevel++;
            } else {
                // If we intersected, pull back the ray to the point of intersection (for that mipLevel)
                float t = (raySample.z - ZBufferValue) / screenSpaceReflectionVec.z;
                raySample -= screenSpaceReflectionVec * t;

                // And, then perform successive test on the next lower mip level.
                // Once we've got a valid intersection with mip 0, we've found our intersection point
                mipLevel--;
            }

            iterations++;
            if (iterations > 1000) {
                return false;
            }
        }

        return true;
    }

    void DeferredSceneRenderer::debugSSR() {

        auto& camera = *mScene->camera();

        auto depthSampler = mGBuffer->HiZBuffer.sampleTexels();
        auto gBufferSampler = mGBuffer->albedoRoughnessMetalnessAONormal.sampleTexels();

        for (int y = 0; y < mGBuffer->HiZBuffer.size().height; y += 1.0) {
            for (int x = 0; x < mGBuffer->HiZBuffer.size().width; x += 1.0) {

                glm::vec2 pixelCoords(x, y);
                glm::vec2 vTexCoords = pixelCoords / glm::vec2(mSettings.resolution.width, mSettings.resolution.height);
                glm::vec2 currentFragUV = vTexCoords;

                glm::uvec3 gBufferSample = gBufferSampler.sample(x, y);
                GBuffer gBuffer = decodeGBuffer(gBufferSample);
                float depth = depthSampler.sample(x, y);
                glm::vec3 worldPosition = ReconstructWorldPosition(camera, depth, pixelCoords);
                glm::vec3 N = gBuffer.normal;

                // Prerequisites

                glm::vec3 cameraToFrag = normalize(worldPosition - camera.position());

                // ScreenSpacePos --> (screencoord.xy, device_z)
                glm::vec4 screenSpacePos = glm::vec4(currentFragUV, depth, 1.0);

                // Compute world space reflection vector
                glm::vec3 reflectionWorldVec = glm::reflect(cameraToFrag, N);
                //
                //                float attenuationFactor;
                //                if (IsReflectedBackToCamera(reflectionWorldVec, -cameraToFrag, attenuationFactor)) {
                //                    //            return vec3(0.5, 0.0, 0.0);
                //                }

                // Compute second screen space point so that we can get the SS reflection vector
                glm::vec4 pointAlongReflectionVec = glm::vec4(10.0f * reflectionWorldVec + worldPosition, 1.0);
                glm::vec4 screenSpaceReflectionPoint = camera.projectionMatrix() * camera.viewMatrix() * pointAlongReflectionVec;
                screenSpaceReflectionPoint /= screenSpaceReflectionPoint.w;
                screenSpaceReflectionPoint.x = screenSpaceReflectionPoint.x * 0.5 + 0.5; // To [0; 1]
                screenSpaceReflectionPoint.y = screenSpaceReflectionPoint.y * 0.5 + 0.5; // To [0; 1]
                screenSpaceReflectionPoint.z = screenSpaceReflectionPoint.z * 0.5 + 0.5; // To [0; 1]

                // Compute the sreen space reflection vector as the difference of the two screen space points
                glm::vec3 screenSpaceReflectionVec = glm::normalize(glm::vec3(screenSpaceReflectionPoint) - glm::vec3(screenSpacePos));

                glm::vec3 outReflectionColor;
                if (GetReflection(reflectionWorldVec, glm::vec3(screenSpaceReflectionVec), glm::vec3(screenSpacePos), outReflectionColor)) {
                    printf("Success");
                } else {
                    printf("Fail\n");
                }
            }
        }


    }

}
