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
#include "GLVertexArray.hpp"

namespace EARenderer {
    
    class ResourcePool {
    private:
        GLVertexArray<Vertex1P1N2UV1T1BT> mVAO;
        int32_t mTotalVertexCount = 0;

    public:
        PackedLookupTable<Mesh> meshes;
        PackedLookupTable<PBRMaterial> materials;
        
        static ResourcePool& shared();
        
        ResourcePool();

        const GLVertexArray<Vertex1P1N2UV1T1BT>& VAO() const;
        int32_t totalVertexCount() const;

        /**
         Updates VAO's underlying buffer with all of the scene's geometry.
         Updates VBO offsets and vertex counts in sub meshes.
         */
        void transferMeshesToGPU();
    };
    
}

#endif /* ResourcePool_hpp */
