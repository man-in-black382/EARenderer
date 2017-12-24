//
//  SurfelsRenderer.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 24.12.2017.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "SurfelRenderer.hpp"
#include "MeshSampler.hpp"

namespace EARenderer {
    
    SurfelRenderer::SurfelRenderer(Scene* scene, ResourcePool* resourcePool, ID meshInstanceID)
    :
    mScene(scene),
    mResourcePool(resourcePool),
    mMeshInstanceID(meshInstanceID)
    {
        MeshInstance& instance = scene->meshInstances()[meshInstanceID];
        MeshSampler sampler(resourcePool);
        mPoints = sampler.samplePointsOnMesh(instance.meshID());
        mVAO.initialize(mPoints, GLVertexArrayLayoutDescription({
            static_cast<int>(glm::vec3::length() * sizeof(GLfloat))
        }));
        
        glEnable(GL_PROGRAM_POINT_SIZE);
    }
    
    void SurfelRenderer::render() {
        mVAO.bind();
        mGenericGeometryShader.bind();
        
        auto& instance = mScene->meshInstances()[mMeshInstanceID];
        auto mvp = mScene->camera()->viewProjectionMatrix() * instance.transformation().modelMatrix();
        
        mGenericGeometryShader.setModelViewProjectionMatrix(mvp);
        mGenericGeometryShader.setColor(Color::red());
        
        glDrawArrays(GL_POINTS, 0, static_cast<GLint>(mPoints.size()));
    }
    
}
