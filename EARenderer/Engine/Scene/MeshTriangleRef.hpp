//
//  RayHit.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 10.03.2018.
//  Copyright © 2018 MPO. All rights reserved.
//

#ifndef RayHit_hpp
#define RayHit_hpp

#include "PackedLookupTable.hpp"
#include "Triangle3D.hpp"

namespace EARenderer {

    struct MeshTriangleRef {
        ID instanceID;
        ID subMeshID;
        Triangle3D triangle;
    };

}

#endif /* RayHit_hpp */
