//
//  Mesh.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 05.03.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "Mesh.hpp"

namespace EARenderer {
    
#pragma mark - Lifecycle
    
    Mesh::Mesh(Mesh&& that)
    :
    mBoundingBoxVisualizer(std::move(that.mBoundingBoxVisualizer))
    {
        that.mBoundingBoxVisualizer = nullptr;
    }
    
    Mesh& Mesh::operator=(Mesh rhs) {
        swap(rhs);
        return *this;
    }
    
    Mesh::~Mesh() {
        deallocateBoundingBoxVisualizer();
    }
    
#pragma mark - Swap
    
    void Mesh::swap(Mesh& that) {
        std::swap(mName, that.mName);
        std::swap(mSubMeshIDs, that.mSubMeshIDs);
        std::swap(mTransformID, that.mTransformID);
        std::swap(mBoundingBox, that.mBoundingBox);
        std::swap(mIsHighlighted, that.mIsHighlighted);
        std::swap(mBoundingBoxVisualizer, that.mBoundingBoxVisualizer);
    }
    
    void swap(Mesh& lhs, Mesh& rhs) {
        lhs.swap(rhs);
    }
    
#pragma mark - Getters
    
    const std::string& Mesh::name() const {
        return mName;
    }
    
    const std::vector<ID>& Mesh::subMeshIDs() const {
        return mSubMeshIDs;
    }
    
    const ID Mesh::transformID() const {
        return mTransformID;
    }
    
    const Box& Mesh::boundingBox() const {
        return mBoundingBox;
    }
    
    bool Mesh::isHighlighted() const {
        return mIsHighlighted;
    }
    
    const BoxVisualizer* Mesh::boundingBoxVisualizer() const {
        return mBoundingBoxVisualizer;
    }
    
#pragma mark - Setters
    
    void Mesh::setName(const std::string &name) {
        mName = name;
    }
    
    void Mesh::setTransformID(ID transformID) {
        mTransformID = transformID;
    }
    
    void Mesh::setSubMeshIDs(const std::vector<ID> &subMeshIDs) {
        mSubMeshIDs = subMeshIDs;
    }
    
    void Mesh::setBoundingBox(const Box& box) {
        mBoundingBox = box;
        allocateBoundingBoxVisualizer();
    }
    
    void Mesh::setHighlighted(bool isHighlighted) {
        mIsHighlighted = isHighlighted;
    }
    
    void Mesh::allocateBoundingBoxVisualizer() {
        if (mBoundingBoxVisualizer) {
            deallocateBoundingBoxVisualizer();
        }
        mBoundingBoxVisualizer = new BoxVisualizer(mBoundingBox);
    }
    
    void Mesh::deallocateBoundingBoxVisualizer() {
        if (mBoundingBoxVisualizer) {
            delete mBoundingBoxVisualizer;
            mBoundingBoxVisualizer = nullptr;
        }
    }
    
}


