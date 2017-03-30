//
//  SubMesh.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 07.03.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "SubMesh.hpp"

namespace EARenderer {

    SubMesh::SubMesh(const std::vector<Vertex1P1N1UV>& vertices, const std::vector<GLushort>& indices)
    :
    mVertices(vertices),
    mIndices(indices)
    { }
    
    const std::vector<Vertex1P1N1UV>& SubMesh::getVertices() const {
        return mVertices;
    }
    
    const std::vector<GLushort>& SubMesh::getIndices() const {
        return mIndices;
    }

}
