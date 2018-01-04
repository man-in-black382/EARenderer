//
//  TriangleRenderer.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 30.12.2017.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "TriangleRenderer.hpp"

namespace EARenderer {
    
#pragma mark - Lifecycle
    
    TriangleRenderer::TriangleRenderer(Scene *scene, ResourcePool *resourcePool)
    :
    mScene(scene),
    mResourcePool(resourcePool)
    { }
    
#pragma mark - Rendering
    
    void TriangleRenderer::render() {
        glEnable(GL_MULTISAMPLE);
        
        mTriangleRenderingShader.bind();
        mTriangleRenderingShader.setColor(Color::white());
        
        glm::mat4 viewProjection = mScene->camera()->viewProjectionMatrix();
        
        for (ID meshInstanceID : mScene->meshInstances()) {
            auto& instance = mScene->meshInstances()[meshInstanceID];
            auto& subMeshes = mResourcePool->meshes[instance.meshID()].subMeshes();
            
            glm::mat4 mvp = viewProjection * instance.transformation().modelMatrix();
            mTriangleRenderingShader.setModelViewProjectionMatrix(mvp);
            
            for (ID subMeshID : subMeshes) {
                auto& subMesh = subMeshes[subMeshID];
                subMesh.draw();
            }
        }
        
        glDisable(GL_MULTISAMPLE);
    }
    
}
