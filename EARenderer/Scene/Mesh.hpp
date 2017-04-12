//
//  Mesh.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 05.03.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#ifndef Mesh_hpp
#define Mesh_hpp

#include <vector>

#include "Geometry.hpp"
#include "Types.h"

namespace EARenderer {
    
    class Mesh {
    private:
        std::vector<ID> mSubMeshIDs;
        ID mTransformID;
        
    public:
        void setSubMeshIDs(const std::vector<ID>& subMeshIDs);
        const std::vector<ID>& subMeshIDs() const;
        void setTransformID(ID transformID);
        const ID transformID() const;
    };

}

#endif /* Mesh_hpp */
