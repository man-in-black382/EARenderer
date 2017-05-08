//
//  Renderer.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 29.03.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "Renderer.hpp"
#include "GLShader.hpp"
#include <glm/gtc/type_ptr.hpp>

namespace EARenderer {
    
    Renderer::Renderer(GLSLProgramFacility *facility)
    :
    mProgramFacility(facility),
//    mSystemFramebuffer(systemFramebuffer),
    mDepthTexture(Size(1024, 1024)),
    mDepthFramebuffer(Size(1024, 1024))
    {
        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
        
        mDepthFramebuffer.attachTexture(mDepthTexture);
    }

    void Renderer::render(Scene *scene) {
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
            Transform& transform = scene->transforms()[transformID];
            ID lightID = *(scene->lights().begin());
            DirectionalLight& light = scene->lights()[lightID];
            
            mProgramFacility->depthFillerProgram()->flushState();
            mProgramFacility->depthFillerProgram()->setModelMatrix(transform.modelMatrix());
            mProgramFacility->depthFillerProgram()->setViewProjectionMatrix(light.viewProjectionMatrix());
            
            subMesh.draw();
        }
        
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, mDepthFramebuffer.size().width, mDepthFramebuffer.size().height);
        
        mProgramFacility->blinnPhongProgram()->bind();
        
        for (ID subMeshID : scene->subMeshes()) {
            SubMesh& subMesh = scene->subMeshes()[subMeshID];
            ID meshID = subMesh.meshID();
            ID transformID = scene->meshes()[meshID].transformID();
            Transform& transform = scene->transforms()[transformID];
            
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
            
            subMesh.draw();
        }
//
//        mProgramFacility->skyboxProgram()->bind();
//        mProgramFacility->skyboxProgram()->flushState();
//        mProgramFacility->skyboxProgram()->setViewMatrix(scene->camera()->viewMatrix());
//        mProgramFacility->skyboxProgram()->setProjectionMatrix(scene->camera()->projectionMatrix());
//        mProgramFacility->skyboxProgram()->setCubemap(scene->skybox()->cubemap());
//        scene->skybox()->draw();
    }
    
}
