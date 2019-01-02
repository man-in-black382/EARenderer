//
//  CookTorranceMaterial.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 07.10.2017.
//  Copyright © 2017 MPO. All rights reserved.
//

#ifndef PBRMaterial_hpp
#define PBRMaterial_hpp

#include <string>
#include <memory>
#include <variant>
#include <optional>
#include <glm/vec3.hpp>

#include "GLTexture2D.hpp"
#include "Color.hpp"

namespace EARenderer {

    class CookTorranceMaterial {
    public:
        using AlbedoMap             = GLNormalizedTexture2D<GLTexture::Normalized::RGBACompressedRGBAInput>;
        using NormalMap             = GLNormalizedTexture2D<GLTexture::Normalized::RGBCompressedRGBAInput>;
        using MetallnessMap         = GLNormalizedTexture2D<GLTexture::Normalized::RCompressedRGBAInput>;
        using RoughnessMap          = GLNormalizedTexture2D<GLTexture::Normalized::RCompressedRGBAInput>;
        using AmbientOcclusionMap   = GLNormalizedTexture2D<GLTexture::Normalized::RCompressedRGBAInput>;
        using DisplacementMap       = GLNormalizedTexture2D<GLTexture::Normalized::RCompressedRGBAInput>;

    private:
        std::unique_ptr<AlbedoMap> mAlbedoMap;
        std::unique_ptr<NormalMap> mNormalMap;
        std::unique_ptr<MetallnessMap> mMetallicMap;
        std::unique_ptr<RoughnessMap> mRoughnessMap;
        std::unique_ptr<AmbientOcclusionMap> mAmbientOcclusionMap;
        std::unique_ptr<DisplacementMap> mDisplacementMap;

    public:
        CookTorranceMaterial(
                std::variant<std::string, Color> albedo,
                std::variant<std::string, glm::vec3> normal,
                std::variant<std::string, float> metalness,
                std::variant<std::string, float> roughness,
                std::variant<std::string, float> ambientOcclusion,
                std::variant<std::string, float> displacement
        );

        const AlbedoMap *albedoMap() const;

        const NormalMap *normalMap() const;

        const MetallnessMap *metallicMap() const;

        const RoughnessMap *roughnessMap() const;

        const AmbientOcclusionMap *ambientOcclusionMap() const;

        const DisplacementMap *displacementMap() const;
    };

}

#endif /* PBRMaterial_hpp */
