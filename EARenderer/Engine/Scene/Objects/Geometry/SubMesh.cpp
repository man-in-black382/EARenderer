//
//  SubMesh.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 07.03.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "SubMesh.hpp"

#include "GLVertexArrayLayoutDescription.hpp"

namespace EARenderer {

    SubMesh::SubMesh()
    :
    mVAO(GLVertexArray<Vertex1P1N1UV1T1BT>())
    { }
    
    SubMesh::SubMesh(const std::vector<Vertex1P1N1UV1T1BT>& vertices)
    :
    mVertices(vertices),
    mVAO(GLVertexArray<Vertex1P1N1UV1T1BT>())
    {
        finalizeVertexBuffer();
    }
    
#pragma mark - Getters
    
    const std::string& SubMesh::name() const {
        return mName;
    }
    
    std::vector<Vertex1P1N1UV1T1BT>& SubMesh::vertices() {
        return mVertices;
    }
    
    const GLVertexArray<Vertex1P1N1UV1T1BT>& SubMesh::VAO() const {
        return mVAO;
    }
    
#pragma mark - Setters
    
    void SubMesh::setName(const std::string &name) {
        mName = name;
    }
    
#pragma mark - Other methods
    
    void SubMesh::addVertex(const Vertex1P1N1UV1T1BT& vertex) {
        mVertices.push_back(vertex);
    }
    
    void SubMesh::finalizeVertexBuffer() {
        mVAO.initialize(mVertices, GLVertexArrayLayoutDescription({
            static_cast<int>(glm::vec4::length() * sizeof(GLfloat)),
            static_cast<int>(glm::vec3::length() * sizeof(GLfloat)),
            static_cast<int>(glm::vec3::length() * sizeof(GLfloat)),
            static_cast<int>(glm::vec3::length() * sizeof(GLfloat)),
            static_cast<int>(glm::vec3::length() * sizeof(GLfloat))
        }));
    }
    
    void SubMesh::draw() const {
        mVAO.bind();
        glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(mVertices.size()));
    }
}
