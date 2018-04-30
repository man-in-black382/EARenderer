//
//  SubMesh.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 07.03.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "SubMesh.hpp"
#include "Triangle3D.hpp"

namespace EARenderer {

    SubMesh::SubMesh()
    :
    mBoundingBox(AxisAlignedBox3D::MaximumReversed()),
    mArea(0.0),
    mVAO(GLVertexArray<Vertex1P1N2UV1T1BT>())
    { }
    
    SubMesh::SubMesh(const std::vector<Vertex1P1N2UV1T1BT>& vertices)
    :
    mVertices(vertices),
    mBoundingBox(AxisAlignedBox3D::MaximumReversed()),
    mArea(0.0),
    mVAO(GLVertexArray<Vertex1P1N2UV1T1BT>())
    {
        finalizeVertexBuffer();
    }
    
#pragma mark - Getters
    
    const std::string& SubMesh::name() const {
        return mName;
    }
    
    const std::string& SubMesh::materialName() const {
        return mMaterialName;
    }
    
    const std::vector<Vertex1P1N2UV1T1BT>& SubMesh::vertices() const {
        return mVertices;
    }
    
    const AxisAlignedBox3D& SubMesh::boundingBox() const {
        return mBoundingBox;
    }
    
    const GLVertexArray<Vertex1P1N2UV1T1BT>& SubMesh::VAO() const {
        return mVAO;
    }

    std::vector<Vertex1P1N2UV1T1BT>& SubMesh::vertices() {
        return mVertices;
    }

    GLVertexArray<Vertex1P1N2UV1T1BT>& SubMesh::VAO() {
        return mVAO;
    }

    float SubMesh::surfaceArea() const {
        return mArea;
    }
    
#pragma mark - Setters
    
    void SubMesh::setName(const std::string &name) {
        mName = name;
    }
    
    void SubMesh::setMaterialName(const std::string &name) {
        mMaterialName = name;
    }

    
#pragma mark - Other methods
    
    void SubMesh::addVertex(const Vertex1P1N2UV1T1BT& vertex) {
        mBoundingBox.min = glm::min(glm::vec3(vertex.position), mBoundingBox.min);
        mBoundingBox.max = glm::max(glm::vec3(vertex.position), mBoundingBox.max);
        mVertices.push_back(vertex);

        if ((mVertices.size() % 3) == 0) {
            size_t count = mVertices.size();
            auto& v0 = mVertices[count - 3];
            auto& v1 = mVertices[count - 2];
            auto& v2 = mVertices[count - 1];

            Triangle3D triangle(v0.position, v1.position, v2.position);
            mArea += triangle.area();
        }
    }
    
    void SubMesh::finalizeVertexBuffer() {
        mVAO.initialize(mVertices, {
            GLVertexAttribute::UniqueAttribute(sizeof(glm::vec4), glm::vec4::length()),
            GLVertexAttribute::UniqueAttribute(sizeof(glm::vec3), glm::vec3::length()),
            GLVertexAttribute::UniqueAttribute(sizeof(glm::vec2), glm::vec2::length()),
            GLVertexAttribute::UniqueAttribute(sizeof(glm::vec3), glm::vec3::length()),
            GLVertexAttribute::UniqueAttribute(sizeof(glm::vec3), glm::vec3::length()),
            GLVertexAttribute::UniqueAttribute(sizeof(glm::vec3), glm::vec3::length())
        });
    }
    
    void SubMesh::draw() const {
        mVAO.bind();
        glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(mVertices.size()));
    }

    void SubMesh::drawInstanced(size_t instanceCount) {
        mVAO.bind();
        glDrawArraysInstanced(GL_TRIANGLES, 0, static_cast<GLsizei>(mVertices.size()), static_cast<GLsizei>(instanceCount));
    }

}
