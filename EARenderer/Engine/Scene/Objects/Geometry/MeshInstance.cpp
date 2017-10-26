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

namespace EARenderer {
    
#pragma mark - Lifecycle
    
    MeshInstance::MeshInstance(ID meshID)
    :
    mMeshID(meshID)
    {
//        auto& subMeshIDs = Scene::shared().meshes()[meshID].subMeshIDs();
        
    }
    
#pragma mark - Getters
    
    bool MeshInstance::isSelected() const {
        return mIsSelected;
    }
    
    bool MeshInstance::isHighlighted() const {
        return mIsHighlighted;
    }
    
    Transformation MeshInstance::transformation() const {
        Mesh& mesh = ResourcePool::shared().meshes[mMeshID];
        return mTransformation.combinedWith(mesh.baseTransform());
    }
    
    AxisAlignedBox3D MeshInstance::boundingBox() const {
        Mesh& mesh = ResourcePool::shared().meshes[mMeshID];
        return mesh.boundingBox();
    }
    
#pragma mark - Setters
    
    void MeshInstance::setIsSelected(bool selected) {
        mIsSelected = selected;
    }
    
    void MeshInstance::setIsHighlighted(bool highlighted) {
        mIsHighlighted = highlighted;
    }
    
}
