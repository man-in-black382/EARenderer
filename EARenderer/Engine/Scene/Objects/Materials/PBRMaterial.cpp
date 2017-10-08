//
//  PBRMaterial.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 07.10.2017.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "PBRMaterial.hpp"

namespace EARenderer {
    
#pragma mark - Lifecycle
    
    PBRMaterial::PBRMaterial(const std::string& albedoMapPath,
                             const std::string& normalMapPath,
                             const std::string& metallicMapPath,
                             const std::string& roughnessMapPath,
                             const std::string& ambientOcclusionMapPath)
    :
    mAlbedoMap(albedoMapPath.length() != 0 ? new GLTexture2D(albedoMapPath) : nullptr),
    mNormalMap(normalMapPath.length() != 0 ? new GLTexture2D(normalMapPath) : nullptr),
    mMetallicMap(metallicMapPath.length() != 0 ? new GLTexture2D(metallicMapPath) : nullptr),
    mRoughnessMap(roughnessMapPath.length() != 0 ? new GLTexture2D(roughnessMapPath) : nullptr),
    mAmbientOcclusionMap(ambientOcclusionMapPath.length() != 0 ? new GLTexture2D(ambientOcclusionMapPath) : nullptr)
    { }
    
    PBRMaterial::PBRMaterial(PBRMaterial&& that)
    :
    mAlbedoMap(std::move(that.mAlbedoMap)),
    mNormalMap(std::move(that.mNormalMap)),
    mMetallicMap(std::move(that.mMetallicMap)),
    mRoughnessMap(std::move(that.mRoughnessMap)),
    mAmbientOcclusionMap(std::move(that.mAmbientOcclusionMap))
    {
        that.mAlbedoMap = nullptr;
        that.mNormalMap = nullptr;
        that.mMetallicMap = nullptr;
        that.mRoughnessMap = nullptr;
        that.mAmbientOcclusionMap = nullptr;
    }
    
    PBRMaterial& PBRMaterial::operator=(PBRMaterial rhs) {
        swap(rhs);
        return *this;
    }
    
    PBRMaterial::~PBRMaterial() {
        delete mAlbedoMap;
        delete mNormalMap;
        delete mMetallicMap;
        delete mRoughnessMap;
        delete mAmbientOcclusionMap;
    }
    
#pragma mark - Swap
    
    void PBRMaterial::swap(PBRMaterial& that) {
        std::swap(mAlbedoMap, that.mAlbedoMap);
        std::swap(mNormalMap, that.mNormalMap);
        std::swap(mMetallicMap, that.mMetallicMap);
        std::swap(mRoughnessMap, that.mRoughnessMap);
        std::swap(mAmbientOcclusionMap, that.mAmbientOcclusionMap);
    }
    
    void swap(PBRMaterial& lhs, PBRMaterial& rhs) {
        lhs.swap(rhs);
    }
    
#pragma mark - Getters

    const GLTexture2D* PBRMaterial::albedoMap() const {
        return mAlbedoMap;
    }
    
    const GLTexture2D* PBRMaterial::normalMap() const {
        return mNormalMap;
    }
    
    const GLTexture2D* PBRMaterial::metallicMap() const {
        return mMetallicMap;
    }
    
    const GLTexture2D* PBRMaterial::roughnessMap() const {
        return mRoughnessMap;
    }
    
    const GLTexture2D* PBRMaterial::ambientOcclusionMap() const {
        return mAmbientOcclusionMap;
    }
    
}
