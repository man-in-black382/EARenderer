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
    mVAO(GLVertexArray<Vertex1P1N1UV>()),
    mMeshID(0)
    { }
    
    SubMesh::SubMesh(const std::vector<Vertex1P1N1UV>& vertices)
    :
    mVertices(vertices),
    mVAO(GLVertexArray<Vertex1P1N1UV>()),
    mMeshID(0)
    {
        finalizeVertexBuffer();
    }
    
#pragma mark - Getters
    
    const std::string& SubMesh::name() const {
        return mName;
    }
    
    const std::vector<Vertex1P1N1UV>& SubMesh::vertices() const {
        return mVertices;
    }
    
    ID SubMesh::meshID() const {
        return mMeshID;
    }
    
    const GLVertexArray<Vertex1P1N1UV>& SubMesh::VAO() const {
        return mVAO;
    }
    
#pragma mark - Setters
    
    void SubMesh::setMeshID(ID meshID) {
        mMeshID = meshID;
    }
    
    void SubMesh::setName(const std::string &name) {
        mName = name;
    }
    
#pragma mark - Other methods
    
    void SubMesh::addVertex(const Vertex1P1N1UV& vertex) {
        mVertices.push_back(vertex);
    }
    
    void SubMesh::finalizeVertexBuffer() {
        mVAO.initialize(mVertices, GLVertexArrayLayoutDescription({
            static_cast<int>(glm::vec4::length() * sizeof(GLfloat)),
            static_cast<int>(glm::vec3::length() * sizeof(GLfloat)),
            static_cast<int>(glm::vec3::length() * sizeof(GLfloat)) }));
    }
    
    void SubMesh::draw() const {
        mVAO.bind();
        glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(mVertices.size()));
    }
}
