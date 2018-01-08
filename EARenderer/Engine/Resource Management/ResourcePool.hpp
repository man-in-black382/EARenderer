//
//  ResourcePool.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 26.10.2017.
//  Copyright © 2017 MPO. All rights reserved.
//

#ifndef ResourcePool_hpp
#define ResourcePool_hpp

#include "PackedLookupTable.hpp"
#include "Mesh.hpp"
#include "SubMesh.hpp"
#include "PBRMaterial.hpp"

namespace EARenderer {
    
    class ResourcePool {
    public:
        PackedLookupTable<Mesh> meshes;
        PackedLookupTable<PBRMaterial> materials;
        
        static ResourcePool& shared();
        
        ResourcePool();
    };
    
}

#endif /* ResourcePool_hpp */
