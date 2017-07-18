//
//  Material.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 13.04.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "Material.hpp"

namespace EARenderer {
    
    Material::Material(const glm::vec3& ambientReflectances,
                       const glm::vec3& diffuseReflectances,
                       const glm::vec3& specularReflectances,
                       float specularExponent,
                       const std::string& skinPath)
    :
    mAmbientReflectances(ambientReflectances),
    mDiffuseReflectances(diffuseReflectances),
    mSpecularReflectances(specularReflectances),
    mSpecularExponent(specularExponent),
    mSkin(skinPath)
    { }
    
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
    
    const GLTexture2D& Material::skin() const {
        return mSkin;
    }
    
}
