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

#pragma mark - Getters

    const std::string &SubMesh::name() const {
        return mName;
    }

    const std::string &SubMesh::materialName() const {
        return mMaterialName;
    }

    const std::vector<Vertex1P1N2UV1T1BT> &SubMesh::vertices() const {
        return mVertices;
    }

    const AxisAlignedBox3D &SubMesh::boundingBox() const {
        return mBoundingBox;
    }

    std::vector<Vertex1P1N2UV1T1BT> &SubMesh::vertices() {
        return mVertices;
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

    void SubMesh::addVertex(const Vertex1P1N2UV1T1BT &vertex) {
        mBoundingBox.min = glm::min(glm::vec3(vertex.position), mBoundingBox.min);
        mBoundingBox.max = glm::max(glm::vec3(vertex.position), mBoundingBox.max);
        mVertices.push_back(vertex);

        if ((mVertices.size() % 3) == 0) {
            size_t count = mVertices.size();
            auto &v0 = mVertices[count - 3];
            auto &v1 = mVertices[count - 2];
            auto &v2 = mVertices[count - 1];

            Triangle3D triangle(v0.position, v1.position, v2.position);
            mArea += triangle.area();
        }
    }

}
