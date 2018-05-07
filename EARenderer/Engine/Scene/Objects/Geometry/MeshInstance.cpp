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

#include "Macros.h"

namespace EARenderer {
    
#pragma mark - Lifecycle
    
    MeshInstance::MeshInstance(ID meshID)
    :
    mMeshID(meshID)
    {
        // Preallocate material map
        auto& subMeshes = ResourcePool::shared().meshes[meshID].subMeshes();
        for (ID subMeshID : subMeshes) {
            mSubMeshMaterialMap[subMeshID] = IDNotFound;
            mSubMeshDiffuseLightProbeMap[subMeshID] = AbsentProbeIndex;
        }
        
        Mesh& mesh = ResourcePool::shared().meshes[mMeshID];
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

    const glm::mat4& MeshInstance::modelMatrix() const {
        return mModelMatrix;
    }
    
    Transformation MeshInstance::transformation() const {
        return mTransformation;
    }
    
    AxisAlignedBox3D MeshInstance::boundingBox() const {
        Mesh& mesh = ResourcePool::shared().meshes[mMeshID];
        return mesh.boundingBox().transformedBy(mTransformation);
    }
    
    ID MeshInstance::materialIDForSubMeshID(ID subMeshID) const {
        if (mSubMeshMaterialMap.find(subMeshID) == mSubMeshMaterialMap.end()) {
            return IDNotFound;
        }
        return mSubMeshMaterialMap.at(subMeshID);
    }

    MeshInstance::DiffuseLightProbeIndex MeshInstance::diffuseLightProbeIndexForSubMeshID(ID subMeshID) const {
        if (mSubMeshDiffuseLightProbeMap.find(subMeshID) == mSubMeshDiffuseLightProbeMap.end()) {
            return AbsentProbeIndex;
        }
        return mSubMeshDiffuseLightProbeMap.at(subMeshID);
    }

#pragma mark - Setters
    
    void MeshInstance::setIsSelected(bool selected) {
        mIsSelected = selected;
    }
    
    void MeshInstance::setIsHighlighted(bool highlighted) {
        mIsHighlighted = highlighted;
    }
    
    void MeshInstance::setTransformation(const Transformation& transform) {
        mTransformation = transform;
        mModelMatrix = transform.modelMatrix();
    }
    
    void MeshInstance::setMaterialIDForSubMeshID(ID materialID, ID subMeshID) {
        ASSERT(mSubMeshMaterialMap.find(subMeshID) != mSubMeshMaterialMap.end(), "Mesh (ID :" << mMeshID << ") doesn't contain submesh with ID " << subMeshID << " . Therefore cannot set a material for it.");
        mSubMeshMaterialMap[subMeshID] = materialID;
    }
    
    void MeshInstance::setMaterialIDForAllSubmeshes(ID materialID) {
        for (auto& keyValuePair : mSubMeshMaterialMap) {
            ID subMeshID = keyValuePair.first;
            mSubMeshMaterialMap[subMeshID] = materialID;
        }
    }

    void MeshInstance::setDiffuseLightProbeIndexForSubMeshID(DiffuseLightProbeIndex probeIndex, ID subMeshID) {
        ASSERT(mSubMeshDiffuseLightProbeMap.find(subMeshID) != mSubMeshDiffuseLightProbeMap.end(), "Mesh (ID :" << mMeshID << ") doesn't contain submesh with ID " << subMeshID << " . Therefore cannot set light probe index for it.");
        mSubMeshDiffuseLightProbeMap[subMeshID] = probeIndex;
    }
    
}
