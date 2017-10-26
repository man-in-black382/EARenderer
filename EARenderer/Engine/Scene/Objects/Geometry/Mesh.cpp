//
//  Mesh.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 05.03.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "Mesh.hpp"
#include "WavefrontMeshLoader.hpp"

namespace EARenderer {
    
#pragma mark - Lifecycle
    
    Mesh::Mesh(const std::string& filePath)
    :
    mSubMeshes(100)
    {
        WavefrontMeshLoader loader(filePath);
        std::vector<SubMesh> subMeshes;
        
        loader.load(subMeshes, mName, mBoundingBox);
        for (auto& subMesh : subMeshes) {
            mSubMeshes.emplace(std::move(subMesh));
        }
        
        float scaleDown = mBoundingBox.diagonal() * 1.44;
        mBaseTransform.scale = glm::vec3(1.0 / scaleDown);
    }
    
#pragma mark - Swap
    
    void Mesh::swap(Mesh& that) {
        std::swap(mName, that.mName);
        std::swap(mSubMeshes, that.mSubMeshes);
        std::swap(mBoundingBox, that.mBoundingBox);
    }
    
    void swap(Mesh& lhs, Mesh& rhs) {
        lhs.swap(rhs);
    }
    
#pragma mark - Getters
    
    const std::string& Mesh::name() const {
        return mName;
    }
    
    const AxisAlignedBox3D& Mesh::boundingBox() const {
        return mBoundingBox;
    }
    
    const Transformation& Mesh::baseTransform() const {
        return mBaseTransform;
    }
    
    const PackedLookupTable<SubMesh>& Mesh::subMeshes() const {
        return mSubMeshes;
    }
    
#pragma mark - Setters
    
    void Mesh::setName(const std::string &name) {
        mName = name;
    }
    
    void Mesh::setBoundingBox(const AxisAlignedBox3D& box) {
        mBoundingBox = box;
    }
    
}


