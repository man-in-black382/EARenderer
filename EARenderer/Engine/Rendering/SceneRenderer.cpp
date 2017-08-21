//
//  Renderer.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 29.03.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "SceneRenderer.hpp"
#include "GLShader.hpp"
#include "Vertex1P4.hpp"
#include "FrustumCascadeBuilder.hpp"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace EARenderer {
    
#pragma mark - Lifecycle
    
    SceneRenderer::SceneRenderer(Scene* scene)
    :
    mScene(scene),
    mCascadedShadowMaps(Size2D(2048, 2048), mNumberOfCascades),
    mShadowCubeMap(Size2D(2048, 2048)),
    mDepthFramebuffer(Size2D(2048, 2048))
    {
        ID lightID = *(mScene->directionalLights().begin());
        DirectionalLight& light = mScene->directionalLights()[lightID];
        mCascades = FrustumCascadeBuilder::subfrustumsForCameraInLightSpace(*mScene->camera(), light, mNumberOfCascades);
    }
    
#pragma mark - Setters
    
    void SceneRenderer::setDefaultRenderComponentsProvider(DefaultRenderComponentsProviding *provider) {
        mDefaultRenderComponentsProvider = provider;
    }
    
    void SceneRenderer::setMeshHiglightEnabled(bool enabled, ID meshID) {
        if (enabled) {
            mMeshesToHighlight.insert(meshID);
        } else {
            mMeshesToHighlight.erase(meshID);
        }
    }
    
    void SceneRenderer::disableMeshesHighlight() {
        mMeshesToHighlight.clear();
    }
    
#pragma mark - Math
    
    bool SceneRenderer::raySelectsMesh(const Ray3D& ray, ID& meshID) {
        float minimumDistance = std::numeric_limits<float>::max();
        ID closestMeshID = IDNotFound;
        
        for (ID id : mScene->meshes()) {
            Mesh& mesh = mScene->meshes()[id];
            Transformation& transformation = mScene->transforms()[mesh.transformID()];
            glm::mat4 modelMatrix = transformation.modelMatrix();
            Ray3D meshLocalSpaceRay = ray.transformedBy(glm::inverse(modelMatrix));
            
            float distance = 0;
            if (meshLocalSpaceRay.intersectsAAB(mesh.boundingBox(), distance)) {
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
    
    void SceneRenderer::renderShadowMapsForDirectionalLights(const FrustumCascades& cascades) {
        // Fill shadow map
        mDirectionalDepthShader.bind();
        
        for (uint8_t cascade = 0; cascade < cascades.splits.size(); cascade++) {
            mDepthFramebuffer.attachTextureLayer(mCascadedShadowMaps, cascade);
            glClear(GL_DEPTH_BUFFER_BIT);
            
            for (ID subMeshID : mScene->subMeshes()) {
                SubMesh& subMesh = mScene->subMeshes()[subMeshID];
                ID meshID = subMesh.meshID();
                ID transformID = mScene->meshes()[meshID].transformID();
                Transformation& transform = mScene->transforms()[transformID];
                
                mDirectionalDepthShader.flushState();
                mDirectionalDepthShader.setModelMatrix(transform.modelMatrix());
                mDirectionalDepthShader.setViewProjectionMatrix(cascades.lightViewProjections[cascade]);

                subMesh.draw();
            }
        }
    }
    
    void SceneRenderer::renderShadowMapsForPointLights() {
        mOmnidirectionalDepthShader.bind();
        mDepthFramebuffer.attachTexture(mShadowCubeMap);
        
        glClear(GL_DEPTH_BUFFER_BIT);
        
        for (ID subMeshID : mScene->subMeshes()) {
            SubMesh& subMesh = mScene->subMeshes()[subMeshID];
            ID meshID = subMesh.meshID();
            ID transformID = mScene->meshes()[meshID].transformID();
            Transformation& transform = mScene->transforms()[transformID];
            ID lightID = *(mScene->pointLights().begin());
            PointLight& light = mScene->pointLights()[lightID];
            
            mOmnidirectionalDepthShader.flushState();
            mOmnidirectionalDepthShader.setModelMatrix(transform.modelMatrix());
            mOmnidirectionalDepthShader.setLight(light);
            
            subMesh.draw();
        }
    }
    
    void SceneRenderer::renderDirectionalLighting(const FrustumCascades& cascades) {
        ID lightID = *(mScene->directionalLights().begin());
        DirectionalLight& light = mScene->directionalLights()[lightID];
        
        mDirectionalBlinnPhongShader.bind();
        mDirectionalBlinnPhongShader.flushState();
        mDirectionalBlinnPhongShader.setCamera(*(mScene->camera()));
        mDirectionalBlinnPhongShader.setDirectionalLight(light);
        mDirectionalBlinnPhongShader.setShadowMaps(mCascadedShadowMaps);
        mDirectionalBlinnPhongShader.setShadowCascades(cascades);
                
        for (ID subMeshID : mScene->subMeshes()) {
            SubMesh& subMesh = mScene->subMeshes()[subMeshID];
            ID meshID = subMesh.meshID();
            Mesh& mesh = mScene->meshes()[meshID];
            ID transformID = mesh.transformID();
            Transformation& transform = mScene->transforms()[transformID];

            mDirectionalBlinnPhongShader.setModelMatrix(transform.modelMatrix());

            ID materialID = *(mScene->materials().begin());
            Material& material = mScene->materials()[materialID];

            mDirectionalBlinnPhongShader.setMaterial(material);
            mDirectionalBlinnPhongShader.setHighlightColor(mesh.isHighlighted() ? Color::gray() : Color::black());

            subMesh.draw();
        }
    }
    
    void SceneRenderer::renderPointLighting() {
        mOmnidirectionalBlinnPhongShader.bind();
        
        for (ID subMeshID : mScene->subMeshes()) {
            SubMesh& subMesh = mScene->subMeshes()[subMeshID];
            ID meshID = subMesh.meshID();
            Mesh& mesh = mScene->meshes()[meshID];
            ID transformID = mesh.transformID();
            Transformation& transform = mScene->transforms()[transformID];
            
            ID lightID = *(mScene->directionalLights().begin());
            PointLight& light = mScene->pointLights()[lightID];
            
            mOmnidirectionalBlinnPhongShader.flushState();
            mOmnidirectionalBlinnPhongShader.setModelMatrix(transform.modelMatrix());
            mOmnidirectionalBlinnPhongShader.setCamera(*(mScene->camera()));
            mOmnidirectionalBlinnPhongShader.setPointLight(light);
            mOmnidirectionalBlinnPhongShader.setShadowMap(mShadowCubeMap);
            
            ID materialID = *(mScene->materials().begin());
            Material& material = mScene->materials()[materialID];
            
            mOmnidirectionalBlinnPhongShader.setMaterial(material);
            
            subMesh.draw();
        }
    }
    
    void SceneRenderer::renderSkybox() {
        mSkyboxShader.bind();
        mSkyboxShader.flushState();
        mSkyboxShader.setViewMatrix(mScene->camera()->viewMatrix());
        mSkyboxShader.setProjectionMatrix(mScene->camera()->projectionMatrix());
        mSkyboxShader.setCubemap(mScene->skybox()->cubemap());
        mScene->skybox()->draw();
    }
    
    void SceneRenderer::render() {
        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
        glClearColor(0.0, 0.0, 0.0, 1.0);
        
        mDepthFramebuffer.bind();
        mDepthFramebuffer.viewport().apply();
        
        ID lightID = *(mScene->directionalLights().begin());
        DirectionalLight& light = mScene->directionalLights()[lightID];
        mCascades = FrustumCascadeBuilder::subfrustumsForCameraInLightSpace(*mScene->camera(), light, mNumberOfCascades);

        renderShadowMapsForDirectionalLights(mCascades);
//        renderShadowMapsForPointLights();
        
        if (mDefaultRenderComponentsProvider) {
            mDefaultRenderComponentsProvider->bindSystemFramebuffer();
            mDefaultRenderComponentsProvider->defaultViewport().apply();
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
        renderDirectionalLighting(mCascades);
//        renderPointLighting();
//        renderSkybox();
        
        //////////////////////////////////
        //////////////// DEBUG /////////////////
        //////////////////////////////////
        
//        glDisable(GL_DEPTH_TEST);
//
//        mFSQuadShader.bind();
//
//        float width = mDefaultRenderComponentsProvider->defaultViewport().frame().size.width / 3.f;
//        float aspect = mDefaultRenderComponentsProvider->defaultViewport().aspectRatio();
//        for (int i = 0; i < mCascades.splits.size(); i++) {
//            mFSQuadShader.flushState();
//            mFSQuadShader.setTexture(mCascadedShadowMaps, i);
//            GLViewport miniport{ Rect2D{ { width * i, 0.f }, { width, width / aspect } } };
//            miniport.apply();
//            glDrawArrays(GL_TRIANGLES, 0, 4);
//        }
//
//        mGenericShader.bind();
//        mGenericShader.flushState();
//        GLViewport miniport{ Rect2D{ { 0.f, width / aspect }, { width, width / aspect } } };
//        miniport.apply();
//        glm::mat4 view = glm::lookAt(glm::vec3{ 0.0, 1.0, 0.0 }, { 0.0, 0.0, 0.0 }, { 0.0, 0.0, -1.0 });
//        glm::mat4 proj = glm::ortho(-5.f, 5.f, -5.f, 5.f, -5.f, 5.f);
//
//        for (int i = 0; i < mCascades.splits.size(); i++) {
//            BoxVisualizer boxVisualizer{ mCascades.boxes[i] };
//
//            mGenericShader.setModelViewProjectionMatrix(proj * view);
//            mGenericShader.setColor(Color::red());
//            boxVisualizer.draw();
//
//            BoxVisualizer frustumVisualizer{ mCascades.cornerPoints[i] };
//            mGenericShader.setModelViewProjectionMatrix(proj * view);
//            mGenericShader.setColor(Color::green());
//            frustumVisualizer.draw();
//        }
//
//        mDirectionalBlinnPhongShader.bind();
//        mDirectionalBlinnPhongShader.flushState();
//        glUniformMatrix4fv(glGetUniformLocation(mDirectionalBlinnPhongShader.name(), "uCameraSpaceMat"), 1, GL_FALSE, glm::value_ptr(proj * view));
//        glUniform3fv(glGetUniformLocation(mDirectionalBlinnPhongShader.name(), "uCameraPosition"), 1, glm::value_ptr(glm::vec3{ 0.0, 1.0, 0.0 }));
//        mDirectionalBlinnPhongShader.setDirectionalLight(light);
//        mDirectionalBlinnPhongShader.setShadowMaps(mCascadedShadowMaps);
//        mDirectionalBlinnPhongShader.setShadowCascades(mCascades);
//
//        for (ID subMeshID : mScene->subMeshes()) {
//            SubMesh& subMesh = mScene->subMeshes()[subMeshID];
//            ID meshID = subMesh.meshID();
//            Mesh& mesh = mScene->meshes()[meshID];
//            ID transformID = mesh.transformID();
//            Transformation& transform = mScene->transforms()[transformID];
//
//            mDirectionalBlinnPhongShader.setModelMatrix(transform.modelMatrix());
//
//            ID materialID = *(mScene->materials().begin());
//            Material& material = mScene->materials()[materialID];
//
//            mDirectionalBlinnPhongShader.setMaterial(material);
//            mDirectionalBlinnPhongShader.setHighlightColor(mesh.isHighlighted() ? Color::gray() : Color::black());
//
//            subMesh.draw();
//        }
//        
//        if (mDefaultRenderComponentsProvider) {
//            mDefaultRenderComponentsProvider->defaultViewport().apply();
//        }
        
    }
    
}
