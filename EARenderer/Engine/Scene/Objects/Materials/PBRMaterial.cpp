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
    mAlbedoMap(GLTextureFactory::LoadLDRImage<GLTexture::Normalized::RGBACompressed>(albedoMapPath)),
    mNormalMap(GLTextureFactory::LoadLDRImage<GLTexture::Normalized::RGBCompressed>(normalMapPath)),
    mMetallicMap(GLTextureFactory::LoadLDRImage<GLTexture::Normalized::RCompressed>(metallicMapPath)),
    mRoughnessMap(GLTextureFactory::LoadLDRImage<GLTexture::Normalized::RCompressed>(roughnessMapPath)),
    mAmbientOcclusionMap(GLTextureFactory::LoadLDRImage<GLTexture::Normalized::RCompressed>(ambientOcclusionMapPath)),
    mDisplacementMap(GLTextureFactory::LoadLDRImage<GLTexture::Normalized::RCompressed>(displacementMapPath))
    {
        if (mAlbedoMap) mAlbedoMap->generateMipMaps();
        if (mNormalMap) mNormalMap->generateMipMaps();
        if (mMetallicMap) mMetallicMap->generateMipMaps();
        if (mRoughnessMap) mRoughnessMap->generateMipMaps();
        if (mAmbientOcclusionMap) mAmbientOcclusionMap->generateMipMaps();
        if (mDisplacementMap) mDisplacementMap->generateMipMaps();
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
