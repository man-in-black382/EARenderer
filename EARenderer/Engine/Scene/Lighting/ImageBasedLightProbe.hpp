//
// Created by Pavlo Muratov on 2019-01-20.
// Copyright (c) 2019 MPO. All rights reserved.
//

#ifndef EARENDERER_IMAGEBASEDLIGHTPROBE_HPP
#define EARENDERER_IMAGEBASEDLIGHTPROBE_HPP

#include "GLTexture2D.hpp"
#include "GLTextureCubemap.hpp"
#include <memory>

namespace EARenderer {

    class ImageBasedLightProbeGenerator;

    class ImageBasedLightProbe {
    private:
        friend class ImageBasedLightProbeGenerator;

        std::unique_ptr<GLFloatTextureCubemap<GLTexture::Float::RGB16F>> mSpecularIrradiance;
        std::unique_ptr<GLFloatTextureCubemap<GLTexture::Float::RGB16F>> mDiffuseIrradiance;
        std::shared_ptr<GLFloatTexture2D<GLTexture::Float::RG16F>> mBRDFIntegrationMap;
        int8_t mSpecularIrradianceMipCount;

        ImageBasedLightProbe(size_t specularMapResolution, size_t diffuseMapResolution);

    public:
        const GLFloatTextureCubemap<GLTexture::Float::RGB16F>& specularIrradiance() const;

        const GLFloatTextureCubemap<GLTexture::Float::RGB16F>& diffuseIrradiance() const;

        const GLFloatTexture2D<GLTexture::Float::RG16F>& BRDFIntegrationMap() const;

        int8_t specularIrradianceMipCount() const;
    };

}

#endif //EARENDERER_IMAGEBASEDLIGHTPROBE_HPP
