//
//  BoxRenderer.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 08.03.2018.
//  Copyright © 2018 MPO. All rights reserved.
//

#include "BoxRenderer.hpp"

namespace EARenderer {

#pragma mark - Lifecycle

    BoxRenderer::BoxRenderer(const Camera *camera, const std::vector<AxisAlignedBox3D> &boxes)
            : mBoxSidesRenderingShader(GLSLCubeRendering::Mode::Sides),
              mBoxEdgesRenderingShader(GLSLCubeRendering::Mode::Edges),
              mCamera(camera),
              mVAO(GLVertexArray<glm::vec3>::Create(
                      mPoints,
                      std::vector<GLVertexAttribute>{GLVertexAttribute::UniqueAttribute(sizeof(glm::vec3), glm::vec3::length())}
              )) {

        for (auto &box : boxes) {
            mPoints.push_back(box.min);
            mPoints.push_back(box.max);
        }
    }

#pragma mark - Rendering

    void BoxRenderer::render(Mode renderingMode, const glm::mat4 &boxTransform) {
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_BLEND);
        glDisable(GL_CULL_FACE);

        mVAO.bind();

        auto mvp = mCamera->viewProjectionMatrix() * boxTransform;

        if (renderingMode == Mode::Edges || renderingMode == Mode::Full) {
            mBoxEdgesRenderingShader.bind();
            mBoxEdgesRenderingShader.setViewProjectionMatrix(mvp);
            mBoxEdgesRenderingShader.setColor(Color(1.0, 0.4, 0.7, 1.0));
            glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(mPoints.size()));
        }

        if (renderingMode == Mode::Sides || renderingMode == Mode::Full) {
            mBoxSidesRenderingShader.bind();
            mBoxSidesRenderingShader.setViewProjectionMatrix(mvp);
            mBoxSidesRenderingShader.setColor(Color(0.5, 0.6, 0.8, 0.4));
            glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(mPoints.size()));
        }

        glDisable(GL_BLEND);
        glEnable(GL_CULL_FACE);
    }

}
