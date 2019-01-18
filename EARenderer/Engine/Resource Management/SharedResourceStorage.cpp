//
//  SharedResourceStorage.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 26.10.2017.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "SharedResourceStorage.hpp"
#include "Scene.hpp"

namespace EARenderer {

#pragma mark - Lifecycle

    SharedResourceStorage::SharedResourceStorage() : mMeshes(100), mCookTorranceMaterials(100), mEmissiveMaterials(100) {}

#pragma mark - Getters

    const GLVertexArray<Vertex1P1N2UV1T1BT> *SharedResourceStorage::meshVAO() const {
        return mVAO.get();
    }

    int32_t SharedResourceStorage::totalVertexCount() const {
        return mTotalVertexCount;
    }

#pragma mark -

    ID SharedResourceStorage::addMesh(Mesh &&mesh) {
        return mMeshes.insert(std::move(mesh));
    }

    MaterialReference SharedResourceStorage::addMaterial(CookTorranceMaterial &&material) {
        ID id = mCookTorranceMaterials.insert(std::move(material));
        return std::make_pair(MaterialType::CookTorrance, id);
    }

    MaterialReference SharedResourceStorage::addMaterial(EmissiveMaterial &&material) {
        ID id = mEmissiveMaterials.insert(std::move(material));
        return std::make_pair(MaterialType::Emissive, id);
    }

    const Mesh &SharedResourceStorage::mesh(ID meshID) const {
        return mMeshes[meshID];
    }

    const CookTorranceMaterial &SharedResourceStorage::cookTorranceMaterial(ID materialID) const {
        return mCookTorranceMaterials[materialID];
    }

    const EmissiveMaterial &SharedResourceStorage::emissiveMaterial(ID materialID) const {
        return mEmissiveMaterials[materialID];
    }

    CookTorranceMaterial &SharedResourceStorage::cookTorranceMaterial(ID materialID) {
        return mCookTorranceMaterials[materialID];
    }

    EmissiveMaterial &SharedResourceStorage::emissiveMaterial(ID materialID) {
        return mEmissiveMaterials[materialID];
    }

}
