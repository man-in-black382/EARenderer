//
//  MeshNormalVisualizer.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 09.04.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "MeshNormalVisualizer.hpp"

#include <glm/gtx/transform.hpp>

#include "GLVertexArrayLayoutDescription.hpp"

namespace EARenderer {
    
    MeshNormalVisualizer::MeshNormalVisualizer(SubMesh& subMesh) {
        for (auto& vertex : subMesh.vertices()) {
            mVertices.emplace_back(vertex.mPosition);
            mVertices.emplace_back(glm::vec4(vertex.mNormal.x, vertex.mNormal.y, vertex.mNormal.z, 1.0) * 0.05f + vertex.mPosition);
        }
        mVAO = new GLVertexArray<Vertex1P>();
        mVAO->initialize(mVertices, GLVertexArrayLayoutDescription({ static_cast<int>(glm::vec4::length() * sizeof(GLfloat)) }));
    }
    
    MeshNormalVisualizer::MeshNormalVisualizer(MeshNormalVisualizer&& that)
    :
    mVertices(std::move(that.mVertices)),
    mVAO(std::move(that.mVAO))
    {
        that.mVAO = nullptr;
    }
    
    MeshNormalVisualizer& MeshNormalVisualizer::operator=(MeshNormalVisualizer &&rhs) {
        mVertices = std::move(rhs.mVertices);
        mVAO = std::move(rhs.mVAO);
        
        rhs.mVAO = nullptr;
        
        return *this;
    }
    
    MeshNormalVisualizer::~MeshNormalVisualizer() {
        delete mVAO;
    }
    
    const GLVertexArray<Vertex1P> * MeshNormalVisualizer::VAO() const {
        return mVAO;
    }
    
    void MeshNormalVisualizer::draw() const {
        mVAO->bind();
        glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(mVertices.size()));
    }
    
}
