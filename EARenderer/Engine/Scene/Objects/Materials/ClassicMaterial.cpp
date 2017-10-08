//
//  Material.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 13.04.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "ClassicMaterial.hpp"

namespace EARenderer {
    
#pragma mark - Lifecycle
    
    ClassicMaterial::ClassicMaterial(const glm::vec3& ambientReflectances,
                       const glm::vec3& diffuseReflectances,
                       const glm::vec3& specularReflectances,
                       float specularExponent,
                       const std::string& skinPath,
                       const std::string& normalMapPath)
    :
    mAmbientReflectances(ambientReflectances),
    mDiffuseReflectances(diffuseReflectances),
    mSpecularReflectances(specularReflectances),
    mSpecularExponent(specularExponent),
    mSkin(skinPath.length() != 0 ? new GLTexture2D(skinPath) : nullptr),
    mNormalMap(normalMapPath.length() != 0 ? new GLTexture2D(normalMapPath) : nullptr)
    { }
    
    ClassicMaterial::ClassicMaterial(ClassicMaterial&& that)
    :
    mAmbientReflectances(std::move(that.mAmbientReflectances)),
    mDiffuseReflectances(std::move(that.mDiffuseReflectances)),
    mSpecularReflectances(std::move(that.mSpecularReflectances)),
    mSpecularExponent(std::move(that.mSpecularExponent)),
    mSkin(std::move(that.mSkin)),
    mNormalMap(std::move(that.mNormalMap))
    {
        that.mSkin = nullptr;
        that.mNormalMap = nullptr;
    }
    
    ClassicMaterial& ClassicMaterial::operator=(ClassicMaterial rhs) {
        swap(rhs);
        return *this;
    }
    
    ClassicMaterial::~ClassicMaterial() {
        delete mSkin;
        delete mNormalMap;
    }
    
#pragma mark - Swap
    
    void ClassicMaterial::swap(ClassicMaterial& that) {
        std::swap(mAmbientReflectances, that.mAmbientReflectances);
        std::swap(mDiffuseReflectances, that.mDiffuseReflectances);
        std::swap(mSpecularReflectances, that.mSpecularReflectances);
        std::swap(mSpecularExponent, that.mSpecularExponent);
        std::swap(mSkin, that.mSkin);
        std::swap(mNormalMap, that.mNormalMap);
    }
    
    void swap(ClassicMaterial& lhs, ClassicMaterial& rhs) {
        lhs.swap(rhs);
    }
    
#pragma mark - Getters
    
    const glm::vec3& ClassicMaterial::ambientReflectances() const {
        return mAmbientReflectances;
    }
    
    const glm::vec3& ClassicMaterial::diffuseReflectances() const {
        return mDiffuseReflectances;
    }
    
    const glm::vec3& ClassicMaterial::specularReflectances() const {
        return mSpecularReflectances;
    }
    
    float ClassicMaterial::specularExponent() const {
        return mSpecularExponent;
    }
    
    const GLTexture2D* ClassicMaterial::skin() const {
        return mSkin;
    }
    
    const GLTexture2D* ClassicMaterial::normalMap() const {
        return mNormalMap;
    }
    
}
