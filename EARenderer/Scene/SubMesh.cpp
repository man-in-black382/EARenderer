//
//  SubMesh.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 07.03.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "SubMesh.hpp"

namespace EARenderer {

    SubMesh::SubMesh(const std::vector<Vertex1P1N1UV>& vertices)
    :
    mVertices(vertices)
    { }
    
    const std::vector<Vertex1P1N1UV>& SubMesh::vertices() const {
        return mVertices;
    }
    
    void SubMesh::clearStorage() {
//        mVertices.erase(mVertices.begin() + 100, mVertices.begin() + 1000);
//        mVertices.erase(mVertices.begin() + 1000);
//        mVertices.erase(mVertices.begin() + 1050);
//        mVertices.erase(mVertices.begin() + 10550);
//        mVertices.erase(mVertices.begin() + 8090);
    }

}
