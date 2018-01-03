//
//  SurfelsRenderer.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 24.12.2017.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "SurfelRenderer.hpp"
#include "SurfelGenerator.hpp"

namespace EARenderer {
    
    SurfelRenderer::SurfelRenderer(Scene* scene, ResourcePool* resourcePool)
    :
    mScene(scene),
    mResourcePool(resourcePool)
    {
        SurfelGenerator generator(resourcePool);
        generator.generateStaticGeometrySurfels(scene);
        mSurfels = generator.surfels;
        mVAO.initialize(generator.surfels, GLVertexArrayLayoutDescription({
            static_cast<int>(glm::vec3::length() * sizeof(GLfloat)),
            static_cast<int>(glm::vec3::length() * sizeof(GLfloat))
        }));

        glEnable(GL_PROGRAM_POINT_SIZE);
    }
    
    void SurfelRenderer::render() {
        mVAO.bind();
        mSurfelRenderingShader.bind();

        auto mvp = mScene->camera()->viewProjectionMatrix();
        mSurfelRenderingShader.setModelViewProjectionMatrix(mvp);
        mSurfelRenderingShader.setColor(Color(1.0, 0.2, 0.3, 1.0));

        glDrawArrays(GL_POINTS, 0, static_cast<GLint>(mSurfels.size()));
    }
    
}
