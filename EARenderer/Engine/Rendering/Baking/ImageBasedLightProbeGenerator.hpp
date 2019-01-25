//
// Created by Pavlo Muratov on 2019-01-22.
// Copyright (c) 2019 MPO. All rights reserved.
//

#ifndef EARENDERER_IMAGEBASEDLIGHTPROBEGENERATOR_HPP
#define EARENDERER_IMAGEBASEDLIGHTPROBEGENERATOR_HPP

#include "GLSLEquirectangularMapConversion.hpp"
#include "GLSLSpecularRadianceConvolution.hpp"
#include "GLSLDiffuseRadianceConvolution.hpp"
#include "GLSLEquirectangularMapConversion.hpp"
#include "GLSLBRDFIntegration.hpp"
#include "GLFramebuffer.hpp"
#include "GLTexture2D.hpp"

#include <memory>

namespace EARenderer {

    class ImageBasedLightProbe;

    class ImageBasedLightProbeGenerator {
    private:
        size_t mProbeResolution;
        int8_t mSpecularIrradianceMipCount;
        GLFloatTextureCubemap<GLTexture::Float::RGB16F> mConversionCubemap;
        std::shared_ptr<GLFloatTexture2D<GLTexture::Float::RG16F>> mBRDFIntegrationMap;

        GLFramebuffer mFramebuffer;
        GLSLSpecularRadianceConvolution mSpecularRadianceConvolutionShader;
        GLSLDiffuseRadianceConvolution mDiffuseRadianceConvolutionShader;
        GLSLBRDFIntegration mBRDFIntegrationShader;
        GLSLEquirectangularMapConversion mConversionShader;

        void convertEquirectangilarMap(const GLFloatTexture2D<GLTexture::Float::RGB16F> &equirectangularMap);

        void buildDiffuseIrradianceMap(
                const GLFloatTextureCubemap<GLTexture::Float::RGB16F> &radiance,
                GLFloatTextureCubemap<GLTexture::Float::RGB16F> &irradiance
        );

        void buildSpecularIrradianceMap(
                const GLFloatTextureCubemap<GLTexture::Float::RGB16F> &radiance,
                GLFloatTextureCubemap<GLTexture::Float::RGB16F> &irradiance
        );

        void buildBRDFIntegrationMap();

    public:
        ImageBasedLightProbeGenerator(size_t probeResolution);

        ImageBasedLightProbe generate(const GLFloatTexture2D<GLTexture::Float::RGB16F> &HDREquirectangularMap);

        ImageBasedLightProbe generate(GLFloatTextureCubemap<GLTexture::Float::RGB16F> &HDRCubemap);
    };

}

#endif //EARENDERER_IMAGEBASEDLIGHTPROBEGENERATOR_HPP
