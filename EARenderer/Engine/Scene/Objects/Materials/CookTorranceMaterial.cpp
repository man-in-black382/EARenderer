//
//  CookTorranceMaterial.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 07.10.2017.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "CookTorranceMaterial.hpp"
#include "GLTextureFactory.hpp"
#include "Visitor.hpp"

namespace EARenderer {

#pragma mark - Lifecycle

    CookTorranceMaterial::CookTorranceMaterial(
            std::variant<std::string, Color> albedo,
            std::variant<std::string, glm::vec3> normal,
            std::variant<std::string, float> metalness,
            std::variant<std::string, float> roughness,
            std::variant<std::string, float> ambientOcclusion,
            std::variant<std::string, float> displacement) {

        // All std::variant functionality that might throw std::bad_variant_access is marked as available starting with macOS 10.14
        // and that means we can't use std::visit **angry face**
        // https://stackoverflow.com/questions/52310835/xcode-10-call-to-unavailable-function-stdvisit
        //
        if (std::holds_alternative<std::string>(albedo)) {
            mAlbedoMap = GLTextureFactory::LoadLDRImage<GLTexture::Normalized::RGBACompressedRGBAInput>(*std::get_if<std::string>(&albedo));
            mAlbedoMap->generateMipMaps();
        } else {
            auto colorData = std::get_if<Color>(&albedo)->rgba();
            mAlbedoMap = std::make_unique<AlbedoMap>(Size2D(1), &colorData);
        }

        if (std::holds_alternative<std::string>(normal)) {
            mNormalMap = GLTextureFactory::LoadLDRImage<GLTexture::Normalized::RGBCompressedRGBAInput>(*std::get_if<std::string>(&normal));
            mNormalMap->generateMipMaps();
        } else {
            auto normalData = *std::get_if<glm::vec3>(&normal);
            mNormalMap = std::make_unique<NormalMap>(Size2D(1), &normalData);
        }

        if (std::holds_alternative<std::string>(metalness)) {
            mMetallicMap = GLTextureFactory::LoadLDRImage<GLTexture::Normalized::RCompressedRGBAInput>(*std::get_if<std::string>(&metalness));
            mMetallicMap->generateMipMaps();
        } else {
            float value = *std::get_if<float>(&metalness);
            uint8_t unnormalizedValue = uint8_t(value * 255.0);
            mMetallicMap = std::make_unique<MetallnessMap>(Size2D(1), &unnormalizedValue);
        }

        if (std::holds_alternative<std::string>(roughness)) {
            mRoughnessMap = GLTextureFactory::LoadLDRImage<GLTexture::Normalized::RCompressedRGBAInput>(*std::get_if<std::string>(&roughness));
            mRoughnessMap->generateMipMaps();
        } else {
            float value = *std::get_if<float>(&roughness);
            uint8_t unnormalizedValue = uint8_t(value * 255.0);
            mRoughnessMap = std::make_unique<RoughnessMap>(Size2D(1), (&unnormalizedValue));
        }

        if (std::holds_alternative<std::string>(ambientOcclusion)) {
            mAmbientOcclusionMap = GLTextureFactory::LoadLDRImage<GLTexture::Normalized::RCompressedRGBAInput>(*std::get_if<std::string>(&ambientOcclusion));
            mAmbientOcclusionMap->generateMipMaps();
        } else {
            float value = *std::get_if<float>(&ambientOcclusion);
            uint8_t unnormalizedValue = uint8_t(value * 255.0);
            mAmbientOcclusionMap = std::make_unique<AmbientOcclusionMap>(Size2D(1), (&unnormalizedValue));
        }

        if (std::holds_alternative<std::string>(displacement)) {
            mDisplacementMap = GLTextureFactory::LoadLDRImage<GLTexture::Normalized::RCompressedRGBAInput>(*std::get_if<std::string>(&displacement));
            mDisplacementMap->generateMipMaps();
        } else {
            float value = *std::get_if<float>(&displacement);
            uint8_t unnormalizedValue = uint8_t(value * 255.0);
            mDisplacementMap = std::make_unique<DisplacementMap>(Size2D(1), (&unnormalizedValue));
        }

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
