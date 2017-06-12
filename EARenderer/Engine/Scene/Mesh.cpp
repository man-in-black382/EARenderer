//
//  Mesh.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 05.03.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "Mesh.hpp"

namespace EARenderer {
    
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
    
}


