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
#include "CookTorranceMaterial.hpp"
#include "EmissiveMaterial.hpp"
#include "GLVertexArray.hpp"
#include "MaterialType.hpp"

namespace EARenderer {

    class ResourcePool {
    private:
        GLVertexArray<Vertex1P1N2UV1T1BT> mVAO;
        int32_t mTotalVertexCount = 0;

    public:
        PackedLookupTable<Mesh> meshes;
        PackedLookupTable<CookTorranceMaterial> mCookTorranceMaterials;
        PackedLookupTable<EmissiveMaterial> mEmissiveMaterials;

        static ResourcePool &shared();

        ResourcePool();

        const GLVertexArray<Vertex1P1N2UV1T1BT> &VAO() const;

        int32_t totalVertexCount() const;

        /**
         Updates VAO's underlying buffer with all of the scene's geometry.
         Updates VBO offsets and vertex counts in sub meshes.
         */
        void transferMeshesToGPU();

        MaterialReference addMaterial(CookTorranceMaterial &&material);

        MaterialReference addMaterial(EmissiveMaterial &&material);

        const CookTorranceMaterial &cookTorranceMaterial(ID materialID) const;

        const EmissiveMaterial &emissiveMaterial(ID materialID) const;
    };

}

#endif /* ResourcePool_hpp */
