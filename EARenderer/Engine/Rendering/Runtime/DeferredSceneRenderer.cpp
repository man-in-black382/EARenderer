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

    // DEBUG
#define Point2 glm::vec2
#define Point3 glm::vec3
#define Vector2 glm::vec2
#define Vector3 glm::vec3
#define Vector4 glm::vec4
#define float2 glm::vec2
#define float3 glm::vec3
#define int2 glm::ivec2

    glm::vec3 computeClipInfo(float zn, float zf) {
        return glm::vec3(zn * zf, zn - zf, zf);
    }

    /** Given an OpenGL depth buffer value on [0, 1] and description of the projection
     matrix's clipping planes, computes the camera-space (negative) z value.

     See also computeClipInfo in the .cpp file */
    float reconstructCSZ(float depthBufferValue, glm::vec3 clipInfo) {
        return clipInfo[0] / (depthBufferValue * clipInfo[1] + clipInfo[2]);
    }

    void swap(float& a, float& b) {
        float temp = a;
        a = b;
        b = temp;
    }

    float distanceSquared(glm::vec2 a, glm::vec2 b) {
        a -= b;
        return dot(a, a);
    }

    float sign(float value) {
        if (value < 0.0) return -1.0;
        if (value == 0.0) return 0.0;
        return 1.0;
    }

    bool TraceScreenSpaceRay1
    (Point3          csOrigin,
     Vector3         csDirection,
     glm::mat4x4          projectToPixelMatrix,
//     sampler2D       csZBuffer,
     float2          csZBufferSize,
     float           csZThickness,
//     const in bool   csZBufferIsHyperbolic,
     float3          clipInfo,
     float           nearPlaneZ,
     float            stride,
     float           jitterFraction,
     float           maxSteps,
     float        maxRayTraceDistance,
     Point2&      hitPixel,
     int&         which,
     Point3&      csHitPoint) {

        // Clip ray to a near plane in 3D (doesn't have to be *the* near plane, although that would be a good idea)
        float rayLength = ((csOrigin.z + csDirection.z * maxRayTraceDistance) > nearPlaneZ) ?
        (nearPlaneZ - csOrigin.z) / csDirection.z :
        maxRayTraceDistance;
        Point3 csEndPoint = csDirection * rayLength + csOrigin;

        // Project into screen space
        Vector4 H0 = projectToPixelMatrix * Vector4(csOrigin, 1.0);
        Vector4 H1 = projectToPixelMatrix * Vector4(csEndPoint, 1.0);

        // There are a lot of divisions by w that can be turned into multiplications
        // at some minor precision loss...and we need to interpolate these 1/w values
        // anyway.
        //
        // Because the caller was required to clip to the near plane,
        // this homogeneous division (projecting from 4D to 2D) is guaranteed
        // to succeed.
        float k0 = 1.0 / H0.w;
        float k1 = 1.0 / H1.w;

        // Switch the original points to values that interpolate linearly in 2D
        Point3 Q0 = csOrigin * k0;
        Point3 Q1 = csEndPoint * k1;

        // Screen-space endpoints
        Point2 P0 = glm::vec2(H0) * k0;
        Point2 P1 = glm::vec2(H1) * k1;

        // [Optional clipping to frustum sides here]

        // Initialize to off screen
        hitPixel = Point2(-1.0, -1.0);
        which = 0; // Only one layer

        // If the line is degenerate, make it cover at least one pixel
        // to avoid handling zero-pixel extent as a special case later
        P1 += glm::vec2((distanceSquared(P0, P1) < 0.0001) ? 0.01 : 0.0);

        Vector2 delta = P1 - P0;

        // Permute so that the primary iteration is in x to reduce
        // large branches later
        bool permute = false;
        if (abs(delta.x) < abs(delta.y)) {
            // More-vertical line. Create a permutation that swaps x and y in the output
            permute = true;

            // Directly swizzle the inputs
            delta = glm::vec2(delta.y, delta.x);
            P1 = glm::vec2(P1.y, P1.x);
            P0 = glm::vec2(P0.y, P0.x);
        }

        // From now on, "x" is the primary iteration direction and "y" is the secondary one

        float stepDirection = sign(delta.x);
        float invdx = stepDirection / delta.x;
        Vector2 dP = Vector2(stepDirection, invdx * delta.y);

        // Track the derivatives of Q and k
        Vector3 dQ = (Q1 - Q0) * invdx;
        float   dk = (k1 - k0) * invdx;

        // Scale derivatives by the desired pixel stride
        dP *= stride; dQ *= stride; dk *= stride;

        // Offset the starting values by the jitter fraction
        P0 += dP * jitterFraction; Q0 += dQ * jitterFraction; k0 += dk * jitterFraction;

        // Slide P from P0 to P1, (now-homogeneous) Q from Q0 to Q1, and k from k0 to k1
        Point3 Q = Q0;
        float  k = k0;

        // We track the ray depth at +/- 1/2 pixel to treat pixels as clip-space solid
        // voxels. Because the depth at -1/2 for a given pixel will be the same as at
        // +1/2 for the previous iteration, we actually only have to compute one value
        // per iteration.
        float prevZMaxEstimate = csOrigin.z;
        float stepCount = 0.0;
        float rayZMax = prevZMaxEstimate, rayZMin = prevZMaxEstimate;
        float sceneZMax = rayZMax + 1e4;

        // P1.x is never modified after this point, so pre-scale it by
        // the step direction for a signed comparison
        float end = P1.x * stepDirection;

        // We only advance the z field of Q in the inner loop, since
        // Q.xy is never used until after the loop terminates.

        for (Point2 P = P0;
             ((P.x * stepDirection) <= end) &&
             (stepCount < maxSteps) &&
             ((rayZMax < sceneZMax - csZThickness) ||
              (rayZMin > sceneZMax)) &&
             (sceneZMax != 0.0);
             P += dP, Q.z += dQ.z, k += dk, stepCount += 1.0) {

            hitPixel = permute ? glm::vec2(P.y, P.x) : P;

            printf("Hit pixel: %d %d \n", int32_t(hitPixel.x), int32_t(hitPixel.y));

            // The depth range that the ray covers within this loop
            // iteration.  Assume that the ray is moving in increasing z
            // and swap if backwards.  Because one end of the interval is
            // shared between adjacent iterations, we track the previous
            // value and then swap as needed to ensure correct ordering
            rayZMin = prevZMaxEstimate;

            // Compute the value at 1/2 pixel into the future
            rayZMax = (dQ.z * 0.5 + Q.z) / (dk * 0.5 + k);
            prevZMaxEstimate = rayZMax;
            if (rayZMin > rayZMax) { swap(rayZMin, rayZMax); }

            printf("Zmin: %f | Zmax: %f \n", rayZMin, rayZMax);

            // Camera-space z of the background
            sceneZMax = -8.0;
//            sceneZMax = texelFetch(csZBuffer, int2(hitPixel), 0).r;
        } // pixel on ray

        Q.x += dQ.x * stepCount;
        Q.y += dQ.y * stepCount;
        csHitPoint = Q * (1.0f / k);

        // Matches the new loop condition:
        return (rayZMax >= sceneZMax - csZThickness) && (rayZMin <= sceneZMax);
    }

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
    mSurfelsLuminanceMap(surfelData->surfelsGBuffer()->size(), GLTexture::Filter::None),
    mSurfelClustersLuminanceMap(surfelData->surfelClustersGBuffer()->size(), GLTexture::Filter::None),
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

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }

    void DeferredSceneRenderer::averageSurfelClusterLuminances() {
        mSurfelClusterAveragingShader.bind();
        mSurfelClustersLuminanceFramebuffer.bind();
        mSurfelClustersLuminanceFramebuffer.viewport().apply();

        mSurfelClusterAveragingShader.ensureSamplerValidity([&]() {
            mSurfelClusterAveragingShader.setSurfelClustersGBuffer(*mSurfelData->surfelClustersGBuffer());
            mSurfelClusterAveragingShader.setSurfelsLuminaceMap(mSurfelsLuminanceMap);
        });

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }

    void DeferredSceneRenderer::updateGridProbes() {
        mGridProbesUpdateShader.bind();
        mGridProbesUpdateShader.ensureSamplerValidity([&] {
            mGridProbesUpdateShader.setSurfelClustersLuminaceMap(mSurfelClustersLuminanceMap);
            mGridProbesUpdateShader.setProbeProjectionsMetadata(*mDiffuseProbeData->probeClusterProjectionsMetadataBufferTexture());
            mGridProbesUpdateShader.setProjectionClusterIndices(*mDiffuseProbeData->projectionClusterIndicesBufferTexture());
            mGridProbesUpdateShader.setProjectionClusterSphericalHarmonics(*mDiffuseProbeData->projectionClusterSHsBufferTexture());
            mGridProbesUpdateShader.setProbesGridResolution(mProbeGridResolution);
        });

        mGridProbesSHFramebuffer.bind();
        mGridProbesSHFramebuffer.viewport().apply();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, (GLsizei)mProbeGridResolution.z);
    }

    void DeferredSceneRenderer::renderMeshes() {
        mCookTorranceShader.bind();

        mCookTorranceShader.setSettings(mSettings);
        mCookTorranceShader.setCamera(*(mScene->camera()));
        mCookTorranceShader.setViewport(mPostprocessTexturePool->framebuffer().viewport());
        mCookTorranceShader.setLight(mScene->directionalLight());
        mCookTorranceShader.setWorldBoundingBox(mScene->lightBakingVolume());
        mCookTorranceShader.setProbePositions(*mDiffuseProbeData->probePositionsBufferTexture());
        mCookTorranceShader.setFrustumCascades(mShadowCascades);

        mCookTorranceShader.ensureSamplerValidity([&]() {
            mCookTorranceShader.setPreviousFrame(*mPreviousFrame);
            mCookTorranceShader.setGBuffer(*mGBuffer);
            mCookTorranceShader.setExponentialShadowMap(*mDirectionalExponentialShadowMap);
            mCookTorranceShader.setGridProbesSHTextures(mGridProbesSHMaps);
        });

        mPostprocessTexturePool->redirectRenderingToTextures(mFrame, mThresholdFilteredOutputFrame);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }

    void DeferredSceneRenderer::renderFinalImage(const GLFloatTexture2D& image) {
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

        auto viewMat = mScene->camera()->viewMatrix();
        auto projmat = mScene->camera()->projectionMatrix();
        auto pixelMat = mPostprocessTexturePool->framebuffer().viewport().textureSpaceMatrix();

        // DEBUG

        glm::vec3 viewNormal = (viewMat * glm::vec4(1.0, 0.0, 0.0, 0.0));
        glm::vec3 viewPosition = (viewMat * glm::vec4(0.0, 1.5, -1.4, 1.0));

        auto projectedProsition = projmat * glm::vec4(viewPosition, 1.0);
        projectedProsition /= projectedProsition.w;

        glm::vec3 reflected = glm::normalize(glm::reflect(viewPosition, viewNormal));

        Point2 hitTexel;
        Point3 viewSpaceHitPoint;
        int which = 0;

        bool hitDetected = TraceScreenSpaceRay1(viewPosition,
                                                reflected,
                                                pixelMat * projmat,
//                                                uGBufferLinearDepth,
                                                glm::vec2(1280, 720),
                                                0.01,
//                                                false,
                                                glm::vec3(0.0), // Clip info
                                                -0.05, // Near clip plane
                                                1.0, // Stride
                                                0.0, // Jitter fraction
                                                250.0, // Max steps
                                                25.0, // Max trace distance (Far clip plane?)
                                                hitTexel,
                                                which, // Depth layer, will be 0 in our case
                                                viewSpaceHitPoint);

//        vec4 projectedHitPoint = uCameraProjectionMat * vec4(viewSpaceHitPoint, 1.0);
//        projectedHitPoint.xy /= projectedHitPoint.w;
//        projectedHitPoint.xy = projectedHitPoint.xy * 0.5 + 0.5;

        ///////////

        swapFrames();

        renderExponentialShadowMapsForDirectionalLight();
        relightSurfels();
        averageSurfelClusterLuminances();
        updateGridProbes();
        renderMeshes();

        auto bloomOutputTexture = mPostprocessTexturePool->claim();
        mBloomEffect.bloom(mFrame, mThresholdFilteredOutputFrame, bloomOutputTexture, mPostprocessTexturePool, mSettings.bloomSettings);

        auto toneMappingOutputTexture = mPostprocessTexturePool->claim();
        mToneMappingEffect.toneMap(bloomOutputTexture, toneMappingOutputTexture, mPostprocessTexturePool);

        renderFinalImage(*mThresholdFilteredOutputFrame);

        mPostprocessTexturePool->putBack(bloomOutputTexture);
        mPostprocessTexturePool->putBack(toneMappingOutputTexture);
    }

    void DeferredSceneRenderer::renderSkybox() {
        bindDefaultFramebuffer();
        mSkyboxShader.bind();
        mSkyboxShader.ensureSamplerValidity([this]() {
            mSkyboxShader.setViewMatrix(mScene->camera()->viewMatrix());
            mSkyboxShader.setProjectionMatrix(mScene->camera()->projectionMatrix());
            mSkyboxShader.setEquirectangularMap(mScene->skybox()->equirectangularMap());
        });
        mScene->skybox()->draw();
    }

}
