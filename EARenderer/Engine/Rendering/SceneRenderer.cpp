//
//  Renderer.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 29.03.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "SceneRenderer.hpp"
#include "GLShader.hpp"
#include "ResourcePool.hpp"
#include "Vertex1P4.hpp"
#include "Collision.hpp"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace EARenderer {
    
#pragma mark - Lifecycle
    
    SceneRenderer::SceneRenderer(Scene* scene)
    :
    mScene(scene),
    mShadowMaps(Size2D(1024), mNumberOfCascades),
    mShadowCubeMap(Size2D(1024)),
    mDepthFramebuffer(Size2D(1024)),
    mEnvironmentMapCube(Size2D(512)),
    mDiffuseIrradianceMap(Size2D(32)),
    mSpecularIrradianceMap(Size2D(512)),
    mBRDFIntegrationMap(Size2D(512)),
    mIBLFramebuffer(Size2D(512)),
    mLightProbeBuilder(Size2D(128), 50)
    {
        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glClearColor(0.0, 0.0, 0.0, 1.0);
        glClearDepth(1.0);
        glDepthFunc(GL_LEQUAL);
        
        mSpecularIrradianceMap.generateMipmaps();
        
//        convertEquirectangularMapToCubemap();
//        buildDiffuseIrradianceMap();
//        buildSpecularIrradianceMap();
//        buildBRDFIntegrationMap();
        
//        mLightProbeBuilder.buildAndPlaceProbesInScene(scene);
    }
    
#pragma mark - Setters
    
    void SceneRenderer::setDefaultRenderComponentsProvider(DefaultRenderComponentsProviding *provider) {
        mDefaultRenderComponentsProvider = provider;
    }

#pragma mark - Math
    
    bool SceneRenderer::raySelectsMesh(const Ray3D& ray, ID& meshID) {
        float minimumDistance = std::numeric_limits<float>::max();
        ID closestMeshID = IDNotFound;
        
        for (ID id : mScene->meshInstances()) {
            MeshInstance& meshInstance = mScene->meshInstances()[id];
            glm::mat4 modelMatrix = meshInstance.transformation().modelMatrix();
            Ray3D meshLocalSpaceRay = ray.transformedBy(glm::inverse(modelMatrix));
            
            float distance = 0;
            if (Collision::RayAABB(meshLocalSpaceRay, meshInstance.boundingBox(), distance)) {
                // Intersection distance is in the mesh's local space
                // Scale local space ray's direction vector (which is a unit vector) accordingly
                glm::vec3 localScaledDirection = meshLocalSpaceRay.direction * distance;
                // Transform back to world space to obtain real origin -> intersection distance
                glm::vec3 worldScaledDirection = modelMatrix * glm::vec4(localScaledDirection, 1.0);
                float worldDistance = glm::length(worldScaledDirection);
                
                if (worldDistance < minimumDistance) {
                    minimumDistance = worldDistance;
                    closestMeshID = id;
                }
            }
        }
        
        meshID = closestMeshID;
        return closestMeshID != IDNotFound;
    }
    
#pragma mark - Rendering
    
    void SceneRenderer::renderSkybox() {
        mSkyboxShader.bind();
        mSkyboxShader.ensureSamplerValidity([this]() {
            mSkyboxShader.setViewMatrix(mScene->camera()->viewMatrix());
            mSkyboxShader.setProjectionMatrix(mScene->camera()->projectionMatrix());
            mSkyboxShader.setEquirectangularMap(mScene->skybox()->equirectangularMap());
        });
        mScene->skybox()->draw();
    }
    
    void SceneRenderer::renderShadowMapsForDirectionalLights(const FrustumCascades& cascades) {
        // Fill shadow map
        mDirectionalDepthShader.bind();
        mDepthFramebuffer.bind();
        mDepthFramebuffer.viewport().apply();

        for (uint8_t cascade = 0; cascade < cascades.amount; cascade++) {
            mDepthFramebuffer.attachTextureLayer(mShadowMaps, cascade);
            glClear(GL_DEPTH_BUFFER_BIT);

            for (ID meshInstanceID : mScene->meshInstances()) {
                auto& instance = mScene->meshInstances()[meshInstanceID];
                auto& subMeshes = ResourcePool::shared().meshes[instance.meshID()].subMeshes();
                
                mDirectionalDepthShader.setModelMatrix(instance.transformation().modelMatrix());
                mDirectionalDepthShader.setViewProjectionMatrix(cascades.lightViewProjections[cascade]);
                
                for (ID subMeshID : subMeshes) {
                    auto& subMesh = subMeshes[subMeshID];
                    subMesh.draw();
                }
            }
        }
    }
    
#pragma mark Cook-Torrance
    
    void SceneRenderer::convertEquirectangularMapToCubemap() {
        mIBLFramebuffer.bind();
        mIBLFramebuffer.attachTexture(mEnvironmentMapCube);
        mIBLFramebuffer.viewport().apply();
        
        mEqurectangularMapConversionShader.bind();
        mEqurectangularMapConversionShader.ensureSamplerValidity([this]() {
            mEqurectangularMapConversionShader.setEquirectangularEnvironmentMap(mScene->skybox()->equirectangularMap());
        });
        
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDrawArrays(GL_TRIANGLES, 0, 4);
        
        mEnvironmentMapCube.generateMipmaps();
    }
    
    void SceneRenderer::buildDiffuseIrradianceMap() {
        mIBLFramebuffer.bind();
        mIBLFramebuffer.attachTexture(mDiffuseIrradianceMap);
        GLViewport(mDiffuseIrradianceMap.size()).apply();
        
        mRadianceConvolutionShader.bind();
        mRadianceConvolutionShader.setAlgorithm(GLSLRadianceConvolution::Algorithm::diffuse);
        mRadianceConvolutionShader.ensureSamplerValidity([this]() {
            mRadianceConvolutionShader.setEnvironmentRadianceMap(mEnvironmentMapCube);
        });
        
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDrawArrays(GL_TRIANGLES, 0, 4);
    }
    
    void SceneRenderer::buildSpecularIrradianceMap() {
        mIBLFramebuffer.bind();
        GLViewport(mSpecularIrradianceMap.size()).apply();
        
        mRadianceConvolutionShader.bind();
        mRadianceConvolutionShader.setAlgorithm(GLSLRadianceConvolution::Algorithm::specular);
        mRadianceConvolutionShader.ensureSamplerValidity([this]() {
            mRadianceConvolutionShader.setEnvironmentRadianceMap(mEnvironmentMapCube);
        });
        
        for (int16_t mipLevel = 0; mipLevel < mNumberOfIrradianceMips; ++mipLevel) {
            Size2D mipSize = mEnvironmentMapCube.size().transformedBy(glm::vec2(std::pow(0.5, mipLevel)));
            GLViewport(mipSize).apply();
            
            float roughness = (float)mipLevel / (float)(mNumberOfIrradianceMips - 1);
            mRadianceConvolutionShader.setRoughness(roughness);
            
            mIBLFramebuffer.attachTexture(mSpecularIrradianceMap, mipLevel);
            
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glDrawArrays(GL_TRIANGLES, 0, 4);
        }
    }
    
    void SceneRenderer::buildBRDFIntegrationMap() {
        mIBLFramebuffer.bind();
        mIBLFramebuffer.attachTexture(mBRDFIntegrationMap);
        mIBLFramebuffer.viewport().apply();
        
        mBRDFIntegrationShader.bind();
        
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDrawArrays(GL_TRIANGLES, 0, 4);
    }
    
    void SceneRenderer::renderPBRMeshes() {
        
    }
    
#pragma mark - Public access point
    
    void SceneRenderer::render() {
        ID directionalLightID = *(mScene->directionalLights().begin());
        DirectionalLight& directionalLight = mScene->directionalLights()[directionalLightID];
//        FrustumCascades cascades = directionalLight.cascadesForCamera(*mScene->camera(), mNumberOfCascades);
        FrustumCascades cascades = directionalLight.cascadesForWorldBoundingBox(mScene->boundingBox());
        
        renderShadowMapsForDirectionalLights(cascades);
        
        if (mDefaultRenderComponentsProvider) {
            mDefaultRenderComponentsProvider->bindSystemFramebuffer();
            mDefaultRenderComponentsProvider->defaultViewport().apply();
        }
        
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        mCookTorranceShader.bind();
//        mCookTorranceShader.setSphericalHarmonicsBufferTexture(mScene->sphericalHarmonicsBufferTexture());
        mCookTorranceShader.ensureSamplerValidity([&]() {
            mCookTorranceShader.setCamera(*(mScene->camera()));
            mCookTorranceShader.setLight(directionalLight);
            mCookTorranceShader.setShadowMapsUniforms(cascades, mShadowMaps);
//            mCookTorranceShader.setIBLUniforms(mDiffuseIrradianceMap, mSpecularIrradianceMap, mBRDFIntegrationMap, mNumberOfIrradianceMips);
        });
        
        for (ID meshInstanceID : mScene->meshInstances()) {
            auto& instance = mScene->meshInstances()[meshInstanceID];
            auto& subMeshes = ResourcePool::shared().meshes[instance.meshID()].subMeshes();
            
            mCookTorranceShader.setModelMatrix(instance.transformation().modelMatrix());
            
            for (ID subMeshID : subMeshes) {
                auto& subMesh = subMeshes[subMeshID];
                auto& material = ResourcePool::shared().materials[instance.materialIDForSubMeshID(subMeshID)];
                
                mCookTorranceShader.ensureSamplerValidity([this, &material]() {
                    mCookTorranceShader.setMaterial(material);
                });
                
                subMesh.draw();
            }
        }
        
//        renderSkybox();
        mSkyboxShader.bind();
        mSkyboxShader.ensureSamplerValidity([this]() {
            mSkyboxShader.setViewMatrix(mScene->camera()->viewMatrix());
            mSkyboxShader.setProjectionMatrix(mScene->camera()->projectionMatrix());
            mSkyboxShader.setCubemap(mLightProbeBuilder.mEnvironmentMap);
        });
        mScene->skybox()->draw();

        // Drawing shadow map layer
//        GLViewport(Size2D(200, 200)).apply();
//        mFSQuadShader.bind();
//        mFSQuadShader.ensureSamplerValidity([this]() {
//            mFSQuadShader.setTexture(mShadowMaps, 0);
//        });
//        glClear(GL_DEPTH_BUFFER_BIT);
//        glDrawArrays(GL_TRIANGLES, 0, 4);
    }

}
