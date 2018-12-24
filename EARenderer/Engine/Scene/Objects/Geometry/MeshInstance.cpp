//
//  RenderableEntity.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 26.10.2017.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "MeshInstance.hpp"
#include "ResourcePool.hpp"
#include "Scene.hpp"
#include "StringUtils.hpp"

namespace EARenderer {

#pragma mark - Lifecycle

    MeshInstance::MeshInstance(ID meshID)
            : mMeshID(meshID) {

        Mesh &mesh = ResourcePool::shared().meshes[mMeshID];
        mTransformation = mesh.baseTransform();
        mModelMatrix = mTransformation.modelMatrix();
    }

#pragma mark - Getters

    ID MeshInstance::meshID() const {
        return mMeshID;
    }

    bool MeshInstance::isSelected() const {
        return mIsSelected;
    }

    bool MeshInstance::isHighlighted() const {
        return mIsHighlighted;
    }

    const glm::mat4 &MeshInstance::modelMatrix() const {
        return mModelMatrix;
    }

    const Transformation &MeshInstance::transformation() const {
        return mTransformation;
    }

    Transformation& MeshInstance::transformation() {
        return mTransformation;
    }

    AxisAlignedBox3D MeshInstance::boundingBox() const {
        Mesh &mesh = ResourcePool::shared().meshes[mMeshID];
        return mesh.boundingBox().transformedBy(mTransformation);
    }

    std::optional<MaterialReference> MeshInstance::materialReferenceForSubMeshID(ID subMeshID) const {
        if (mSubMeshMaterialMap.find(subMeshID) == mSubMeshMaterialMap.end()) {
            return std::nullopt;
        }
        return mSubMeshMaterialMap.at(subMeshID);
    }

#pragma mark - Setters

    void MeshInstance::setIsSelected(bool selected) {
        mIsSelected = selected;
    }

    void MeshInstance::setIsHighlighted(bool highlighted) {
        mIsHighlighted = highlighted;
    }

    void MeshInstance::setTransformation(const Transformation &transform) {
        mTransformation = transform;
        mModelMatrix = transform.modelMatrix();
    }

    void MeshInstance::setMaterialReferenceForSubMeshID(const MaterialReference &ref, ID subMeshID) {
        mSubMeshMaterialMap[subMeshID] = ref;
    }

}
