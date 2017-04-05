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
    
    Renderer::Renderer(Scene* scene, GLProgram* program)
    :
    mScene(scene),
    mProgram(program)
    {
        auto& subMeshTable = mScene->subMeshes();
        for (ID subMeshID : subMeshTable) {
            auto& subMesh = subMeshTable[subMeshID];
            auto* VAO = new GLVertexArray<Vertex1P1N1UV>();
            VAO->initialize(subMesh.vertices(), GLVertexArrayLayoutDescription({ 4 * sizeof(GLfloat), 2 * sizeof(GLfloat), 3 * sizeof(GLfloat) }));
            mVAOs[subMeshID] = VAO;
        }
    }
    
    void Renderer::render() {
        glClearColor(0.0, 0.0, 0.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);
        
        for (ID subMeshID : mScene->subMeshes()) {
            auto* VAO = mVAOs[subMeshID];
            VAO->bind();
            mProgram->bind();
            auto& camera = mScene->cameras()[mScene->mainCameraID()];
            glUniformMatrix4fv(mProgram->uniformLocation("u_mvp"), 1, GL_FALSE, glm::value_ptr(camera.viewProjectionMatrix()));
            glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(mScene->subMeshes()[subMeshID].vertices().size()));
        }
    }
    
}
