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
        
    Mesh& Mesh::operator=(Mesh rhs) {
        swap(rhs);
        return *this;
    }
    
#pragma mark - Swap
    
    void Mesh::swap(Mesh& that) {
        std::swap(mName, that.mName);
        std::swap(mSubMeshIDs, that.mSubMeshIDs);
        std::swap(mTransformID, that.mTransformID);
        std::swap(mBoundingBox, that.mBoundingBox);
    }
    
    void swap(Mesh& lhs, Mesh& rhs) {
        lhs.swap(rhs);
    }
    
#pragma mark - Getters
    
    const std::string& Mesh::name() const {
        return mName;
    }
    
    bool Mesh::isSelected() const {
        return mIsSelected;
    }
    
    bool Mesh::isHighlighted() const {
        return mIsHighlighted;
    }
    
    const std::vector<ID>& Mesh::subMeshIDs() const {
        return mSubMeshIDs;
    }
    
    const ID Mesh::transformID() const {
        return mTransformID;
    }
    
    const AxisAlignedBox3D& Mesh::boundingBox() const {
        return mBoundingBox;
    }
    
#pragma mark - Setters
    
    void Mesh::setName(const std::string &name) {
        mName = name;
    }
    
    void Mesh::setIsSelected(bool selected) {
        mIsSelected = selected;
    }
    
    void Mesh::setIsHighlighted(bool highlighted) {
        mIsHighlighted = highlighted;
    }
    
    void Mesh::setTransformID(ID transformID) {
        mTransformID = transformID;
    }
    
    void Mesh::setSubMeshIDs(const std::vector<ID> &subMeshIDs) {
        mSubMeshIDs = subMeshIDs;
    }
    
    void Mesh::setBoundingBox(const AxisAlignedBox3D& box) {
        mBoundingBox = box;
    }
    
}


