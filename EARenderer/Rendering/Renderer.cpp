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
    mProgramFacility(facility)
    {
        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    }

    void Renderer::render(Scene *scene) {
        glClearColor(0.0, 0.0, 0.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        mProgramFacility->blinnPhongProgram()->bind();
        
        for (ID subMeshID : scene->subMeshes()) {
            SubMesh& subMesh = scene->subMeshes()[subMeshID];
            ID meshID = subMesh.meshID();
            ID transformID = scene->meshes()[meshID].transformID();
            Transform& transform = scene->transforms()[transformID];
            
            mProgramFacility->blinnPhongProgram()->flushState();
            mProgramFacility->blinnPhongProgram()->setModelMatrix(transform.modelMatrix());
            mProgramFacility->blinnPhongProgram()->setNormalMatrix(transform.modelMatrix());
            mProgramFacility->blinnPhongProgram()->setViewProjectionMatrix(scene->camera()->viewProjectionMatrix());
            
            ID lightID = *(scene->lights().begin());
            Light& light = scene->lights()[lightID];
            
            mProgramFacility->blinnPhongProgram()->setLightPosition(light.position());
            mProgramFacility->blinnPhongProgram()->setLightColor(light.color());
            mProgramFacility->blinnPhongProgram()->setCameraPosition(scene->camera()->position());
            
            ID materialID = *(scene->materials().begin());
            Material& material = scene->materials()[materialID];
            mProgramFacility->blinnPhongProgram()->setMaterial(material);
            
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
