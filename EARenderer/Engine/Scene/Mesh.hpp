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
#include <string>

#include "Geometry.hpp"
#include "PackedLookupTable.inl"

namespace EARenderer {
    
    class Mesh {
    private:
        std::string mName;
        std::vector<ID> mSubMeshIDs;
        ID mTransformID;
        
    public:
        const std::string& name() const;
        const std::vector<ID>& subMeshIDs() const;
        const ID transformID() const;
        
        void setSubMeshIDs(const std::vector<ID>& subMeshIDs);
        void setTransformID(ID transformID);
        void setName(const std::string& name);
    };

}

#endif /* Mesh_hpp */
