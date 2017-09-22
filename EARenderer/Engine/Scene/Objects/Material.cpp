//
//  Material.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 13.04.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "Material.hpp"

namespace EARenderer {
    
#pragma mark - Lifecycle
    
    Material::Material(const glm::vec3& ambientReflectances,
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
    
    Material::Material(Material&& that)
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
    
    Material& Material::operator=(Material rhs) {
        swap(rhs);
        return *this;
    }
    
    Material::~Material() {
        delete mSkin;
        delete mNormalMap;
    }
    
#pragma mark - Swap
    
    void Material::swap(Material& that) {
        std::swap(mAmbientReflectances, that.mAmbientReflectances);
        std::swap(mDiffuseReflectances, that.mDiffuseReflectances);
        std::swap(mSpecularReflectances, that.mSpecularReflectances);
        std::swap(mSpecularExponent, that.mSpecularExponent);
        std::swap(mSkin, that.mSkin);
        std::swap(mNormalMap, that.mNormalMap);
    }
    
    void swap(Material& lhs, Material& rhs) {
        lhs.swap(rhs);
    }
    
#pragma mark - Getters
    
    const glm::vec3& Material::ambientReflectances() const {
        return mAmbientReflectances;
    }
    
    const glm::vec3& Material::diffuseReflectances() const {
        return mDiffuseReflectances;
    }
    
    const glm::vec3& Material::specularReflectances() const {
        return mSpecularReflectances;
    }
    
    float Material::specularExponent() const {
        return mSpecularExponent;
    }
    
    const GLTexture2D* Material::skin() const {
        return mSkin;
    }
    
    const GLTexture2D* Material::normalMap() const {
        return mNormalMap;
    }
    
}
