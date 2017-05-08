//
//  Mesh.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 05.03.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "Mesh.hpp"

namespace EARenderer {

    void Mesh::setSubMeshIDs(const std::vector<ID> &subMeshIDs) {
        mSubMeshIDs = subMeshIDs;
    }
    
    const std::vector<ID>& Mesh::subMeshIDs() const {
        return mSubMeshIDs;
    }
    
    void Mesh::setTransformID(ID transformID) {
        mTransformID = transformID;
    }
    
    const ID Mesh::transformID() const {
        return mTransformID;
    }
    
}


