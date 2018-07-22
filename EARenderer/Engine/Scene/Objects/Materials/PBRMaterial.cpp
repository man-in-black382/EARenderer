//
//  PBRMaterial.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 07.10.2017.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "PBRMaterial.hpp"
#include "GLTextureFactory.hpp"

namespace EARenderer {
    
#pragma mark - Lifecycle
    
    PBRMaterial::PBRMaterial(const std::string& albedoMapPath,
                             const std::string& normalMapPath,
                             const std::string& metallicMapPath,
                             const std::string& roughnessMapPath,
                             const std::string& ambientOcclusionMapPath,
                             const std::string& displacementMapPath)
    :
    mAlbedoMap(GLTextureFactory::LoadImage<GLTexture::Normalized::RGBACompressed>(albedoMapPath)),
    mNormalMap(GLTextureFactory::LoadImage<GLTexture::Normalized::RGBCompressed>(normalMapPath)),
    mMetallicMap(GLTextureFactory::LoadImage<GLTexture::Normalized::RCompressed>(metallicMapPath)),
    mRoughnessMap(GLTextureFactory::LoadImage<GLTexture::Normalized::RCompressed>(roughnessMapPath)),
    mAmbientOcclusionMap(GLTextureFactory::LoadImage<GLTexture::Normalized::RCompressed>(ambientOcclusionMapPath)),
    mDisplacementMap(GLTextureFactory::LoadImage<GLTexture::Normalized::RCompressed>(displacementMapPath))
    {
        if (mAlbedoMap) mAlbedoMap->generateMipmaps();
        if (mNormalMap) mNormalMap->generateMipmaps();
        if (mMetallicMap) mMetallicMap->generateMipmaps();
        if (mRoughnessMap) mRoughnessMap->generateMipmaps();
        if (mAmbientOcclusionMap) mAmbientOcclusionMap->generateMipmaps();
        if (mDisplacementMap) mDisplacementMap->generateMipmaps();
    }
    
#pragma mark - Getters

    const PBRMaterial::AlbedoMap* PBRMaterial::albedoMap() const {
        return mAlbedoMap.get();
    }

    const PBRMaterial::NormalMap* PBRMaterial::normalMap() const {
        return mNormalMap.get();
    }

    const PBRMaterial::MetallnessMap* PBRMaterial::metallicMap() const {
        return mMetallicMap.get();
    }

    const PBRMaterial::RoughnessMap* PBRMaterial::roughnessMap() const {
        return mRoughnessMap.get();
    }

    const PBRMaterial::AmbientOcclusionMap* PBRMaterial::ambientOcclusionMap() const {
        return mAmbientOcclusionMap.get();
    }

    const PBRMaterial::DisplacementMap* PBRMaterial::displacementMap() const {
        return mDisplacementMap.get();
    }
    
}
