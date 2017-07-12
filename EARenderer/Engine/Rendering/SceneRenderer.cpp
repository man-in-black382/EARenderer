//
//  Renderer.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 29.03.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "SceneRenderer.hpp"
#include "GLShader.hpp"
#include <glm/gtc/type_ptr.hpp>

#include "Vertex1P4.hpp"

namespace EARenderer {
    
#pragma mark - Lifecycle
    
    SceneRenderer::SceneRenderer(GLSLProgramFacility *facility)
    :
    mProgramFacility(facility),
    mDepthTexture(Size2D(1024, 1024)),
    mDepthFramebuffer(Size2D(1024, 1024))
    {
        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
        
        mDepthFramebuffer.attachTexture(mDepthTexture);        
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
                    closestMeshID = meshID;
                }
            }
        }
        
        meshID = closestMeshID;
        return closestMeshID != IDNotFound;
    }
    
#pragma mark - Rendering
    
    void SceneRenderer::render(Scene *scene) {
        glClearColor(0.0, 0.0, 0.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // Fill shadow map
        mProgramFacility->depthFillerProgram()->bind();
        
        mDepthFramebuffer.bind();
        glViewport(0, 0, mDepthFramebuffer.size().width, mDepthFramebuffer.size().height);
        
        for (ID subMeshID : scene->subMeshes()) {
            SubMesh& subMesh = scene->subMeshes()[subMeshID];
            ID meshID = subMesh.meshID();
            ID transformID = scene->meshes()[meshID].transformID();
            Transformation& transform = scene->transforms()[transformID];
            ID lightID = *(scene->lights().begin());
            DirectionalLight& light = scene->lights()[lightID];
            
            mProgramFacility->depthFillerProgram()->flushState();
            mProgramFacility->depthFillerProgram()->setModelMatrix(transform.modelMatrix());
            mProgramFacility->depthFillerProgram()->setViewProjectionMatrix(light.viewProjectionMatrix());
            
            subMesh.draw();
        }
        
        if (mDefaultRenderComponentsProvider) {
            mDefaultRenderComponentsProvider->bindSystemFramebuffer();
            mDefaultRenderComponentsProvider->applyDefaultViewport();
        }
        
        mProgramFacility->blinnPhongProgram()->bind();
        
        for (ID subMeshID : scene->subMeshes()) {
            SubMesh& subMesh = scene->subMeshes()[subMeshID];
            ID meshID = subMesh.meshID();
            Mesh& mesh = scene->meshes()[meshID];
            ID transformID = mesh.transformID();
            Transformation& transform = scene->transforms()[transformID];
            
            mProgramFacility->blinnPhongProgram()->flushState();
            mProgramFacility->blinnPhongProgram()->setModelMatrix(transform.modelMatrix());
            mProgramFacility->blinnPhongProgram()->setNormalMatrix(transform.modelMatrix());
            mProgramFacility->blinnPhongProgram()->setCameraSpaceMatrix(scene->camera()->viewProjectionMatrix());
            mProgramFacility->blinnPhongProgram()->setCameraPosition(scene->camera()->position());
            
            ID lightID = *(scene->lights().begin());
            DirectionalLight& light = scene->lights()[lightID];
            
            mProgramFacility->blinnPhongProgram()->setLightPosition(light.position());
            mProgramFacility->blinnPhongProgram()->setLightColor(light.color());
            mProgramFacility->blinnPhongProgram()->setLightSpaceMatrix(light.viewProjectionMatrix());
            mProgramFacility->blinnPhongProgram()->setShadowMap(mDepthTexture);
            
            ID materialID = *(scene->materials().begin());
            Material& material = scene->materials()[materialID];
            mProgramFacility->blinnPhongProgram()->setMaterial(material);
            
//            mProgramFacility->blinnPhongProgram()->setHighlighted(mesh.isHighlighted());
            
            subMesh.draw();
        }
        
        mProgramFacility->skyboxProgram()->bind();
        mProgramFacility->skyboxProgram()->flushState();
        mProgramFacility->skyboxProgram()->setViewMatrix(scene->camera()->viewMatrix());
        mProgramFacility->skyboxProgram()->setProjectionMatrix(scene->camera()->projectionMatrix());
        mProgramFacility->skyboxProgram()->setCubemap(scene->skybox()->cubemap());
        scene->skybox()->draw();
    }
    
}
