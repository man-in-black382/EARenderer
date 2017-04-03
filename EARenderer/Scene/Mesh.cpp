//
//  Mesh.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 05.03.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "Mesh.hpp"

namespace EARenderer {

    Mesh::Mesh(const std::vector<ID>& subMeshIDs)
    :
    mSubMeshIDs(subMeshIDs)
    { }
    
    const std::vector<ID>& Mesh::subMeshIDs() const {
        return mSubMeshIDs;
    }
    
}


