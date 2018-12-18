//
//  CookTorranceMaterial.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 07.10.2017.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "CookTorranceMaterial.hpp"
#include "GLTextureFactory.hpp"

namespace EARenderer {

#pragma mark - Lifecycle

    CookTorranceMaterial::CookTorranceMaterial(const std::string &albedoMapPath,
            const std::string &normalMapPath,
            const std::string &metallicMapPath,
            const std::string &roughnessMapPath,
            const std::string &ambientOcclusionMapPath,
            const std::string &displacementMapPath)
            :
            mAlbedoMap(GLTextureFactory::LoadLDRImage<GLTexture::Normalized::RGBACompressedRGBAInput>(albedoMapPath)),
            mNormalMap(GLTextureFactory::LoadLDRImage<GLTexture::Normalized::RGBCompressedRGBAInput>(normalMapPath)),
            mMetallicMap(GLTextureFactory::LoadLDRImage<GLTexture::Normalized::RCompressedRGBAInput>(metallicMapPath)),
            mRoughnessMap(GLTextureFactory::LoadLDRImage<GLTexture::Normalized::RCompressedRGBAInput>(roughnessMapPath)),
            mAmbientOcclusionMap(GLTextureFactory::LoadLDRImage<GLTexture::Normalized::RCompressedRGBAInput>(ambientOcclusionMapPath)),
            mDisplacementMap(GLTextureFactory::LoadLDRImage<GLTexture::Normalized::RCompressedRGBAInput>(displacementMapPath)) {
        if (mAlbedoMap) mAlbedoMap->generateMipMaps();
        if (mNormalMap) mNormalMap->generateMipMaps();
        if (mMetallicMap) mMetallicMap->generateMipMaps();
        if (mRoughnessMap) mRoughnessMap->generateMipMaps();
        if (mAmbientOcclusionMap) mAmbientOcclusionMap->generateMipMaps();
        if (mDisplacementMap) mDisplacementMap->generateMipMaps();
    }

#pragma mark - Getters

    const CookTorranceMaterial::AlbedoMap *CookTorranceMaterial::albedoMap() const {
        return mAlbedoMap.get();
    }

    const CookTorranceMaterial::NormalMap *CookTorranceMaterial::normalMap() const {
        return mNormalMap.get();
    }

    const CookTorranceMaterial::MetallnessMap *CookTorranceMaterial::metallicMap() const {
        return mMetallicMap.get();
    }

    const CookTorranceMaterial::RoughnessMap *CookTorranceMaterial::roughnessMap() const {
        return mRoughnessMap.get();
    }

    const CookTorranceMaterial::AmbientOcclusionMap *CookTorranceMaterial::ambientOcclusionMap() const {
        return mAmbientOcclusionMap.get();
    }

    const CookTorranceMaterial::DisplacementMap *CookTorranceMaterial::displacementMap() const {
        return mDisplacementMap.get();
    }

}
