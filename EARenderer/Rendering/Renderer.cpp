//
//  Renderer.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 29.03.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "Renderer.hpp"

namespace EARenderer {
    
    Renderer::Renderer(Scene* scene)
    :
    mScene(scene)
    {
        auto& subMeshTable = mScene->getSubMeshes();
        for (ID subMeshID : subMeshTable) {
            auto& subMesh = subMeshTable[subMeshID];
            auto* VAO = new GLVertexArray<Vertex1P1N1UV>();
            VAO->initialize(subMesh.getVertices(), subMesh.getIndices(), GLVertexArrayLayoutDescription({ 4 * sizeof(GLfloat), 2 * sizeof(GLfloat), 3 * sizeof(GLfloat) }));
            mVAOs[subMeshID] = VAO;
        }
    }
    
    void Renderer::render() {
        for (ID subMeshID : mScene->getSubMeshes()) {
            auto* VAO = mVAOs[subMeshID];
            VAO->bind();
            glDrawElements(GL_TRIANGLES, VAO->getElementsCount(), GL_UNSIGNED_SHORT, nullptr);
        }
    }
    
}
