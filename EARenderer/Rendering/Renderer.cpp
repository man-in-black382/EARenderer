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
    { }

    void Renderer::render(Scene *scene) {
        glClearColor(0.0, 0.0, 0.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        auto *blinnPhong = mProgramFacility->blinnPhongProgram();
        blinnPhong->bind();
        
        for (ID subMeshID : scene->subMeshes()) {
            SubMesh& subMesh = scene->subMeshes()[subMeshID];
            ID meshID = subMesh.meshID();
            ID transformID = scene->meshes()[meshID].transformID();
            Transform& transform = scene->transforms()[transformID];
            
            blinnPhong->setModelMatrix(transform.modelMatrix());
            blinnPhong->setNormalMatrix(transform.modelMatrix());
            blinnPhong->setViewProjectionMatrix(scene->camera().viewProjectionMatrix());
            
            ID lightID = *(scene->lights().begin());
            Light& light = scene->lights()[lightID];
            
            blinnPhong->setLightPosition(light.position());
            blinnPhong->setLightColor(light.color());
            
            blinnPhong->setCameraPosition(scene->camera().position());
            blinnPhong->setMaterial({ 0.2, 0.2, 0.2 }, { 1.0, 1.0, 1.0 }, { 1.0, 1.0, 1.0 }, 16);
            
            subMesh.draw();
        }
    }
    
}
