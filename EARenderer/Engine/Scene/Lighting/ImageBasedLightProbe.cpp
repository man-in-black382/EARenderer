//
// Created by Pavlo Muratov on 2019-01-20.
// Copyright (c) 2019 MPO. All rights reserved.
//

#include "ImageBasedLightProbe.hpp"
#include "ImageBasedLightProbeGenerator.hpp"

namespace EARenderer {

    ImageBasedLightProbe::ImageBasedLightProbe(size_t specularMapResolution, size_t diffuseMapResolution)
            : mDiffuseIrradiance(std::make_unique<GLFloatTextureCubemap<GLTexture::Float::RGB16F>>(diffuseMapResolution)),
              mSpecularIrradiance(std::make_unique<GLFloatTextureCubemap<GLTexture::Float::RGB16F>>(specularMapResolution)) {}

    const GLFloatTextureCubemap<GLTexture::Float::RGB16F> &ImageBasedLightProbe::specularIrradiance() const {
        return *mSpecularIrradiance;
    }

    const GLFloatTextureCubemap<GLTexture::Float::RGB16F> &ImageBasedLightProbe::diffuseIrradiance() const {
        return *mDiffuseIrradiance;
    }

    const GLFloatTexture2D<GLTexture::Float::RG16F> &ImageBasedLightProbe::BRDFIntegrationMap() const {
        return *mBRDFIntegrationMap;
    }

    int8_t ImageBasedLightProbe::specularIrradianceMipCount() const {
        return mSpecularIrradianceMipCount;
    }

}