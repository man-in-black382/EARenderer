//
//  SharedResourceStorage.hpp
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
#include "GLUniformBuffer.hpp"

namespace EARenderer {

    class Scene;

    class SharedResourceStorage {
    private:
        int32_t mTotalVertexCount = 0;

        std::unique_ptr<GLVertexArray<Vertex1P1N2UV1T1BT>> mVAO;
        std::unique_ptr<GLUniformBuffer> mUniformBuffer;

        PackedLookupTable<Mesh> mMeshes;
        PackedLookupTable<CookTorranceMaterial> mCookTorranceMaterials;
        PackedLookupTable<EmissiveMaterial> mEmissiveMaterials;

    public:
        SharedResourceStorage();

        const GLVertexArray<Vertex1P1N2UV1T1BT> *meshVAO() const;

        int32_t totalVertexCount() const;

        ID addMesh(Mesh &&mesh);

        MaterialReference addMaterial(CookTorranceMaterial &&material);

        MaterialReference addMaterial(EmissiveMaterial &&material);

        const Mesh &mesh(ID meshID) const;

        const CookTorranceMaterial &cookTorranceMaterial(ID materialID) const;

        const EmissiveMaterial &emissiveMaterial(ID materialID) const;

        CookTorranceMaterial &cookTorranceMaterial(ID materialID);

        EmissiveMaterial &emissiveMaterial(ID materialID);

        template <typename F>
        void iterateMeshes(F f) const {
            std::for_each(std::begin(mMeshes), std::end(mMeshes), f);
        }

        template <typename F>
        void iterateMaterials(F f) const {
//            std::for_each(std::begin(mMeshes), std::end(mMeshes), f);
        }
    };

}

#endif /* ResourcePool_hpp */
