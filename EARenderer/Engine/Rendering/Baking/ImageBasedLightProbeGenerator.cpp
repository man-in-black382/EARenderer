//
// Created by Pavlo Muratov on 2019-01-22.
// Copyright (c) 2019 MPO. All rights reserved.
//

#include "ImageBasedLightProbeGenerator.hpp"
#include "ImageBasedLightProbe.hpp"
#include "Drawable.hpp"

namespace EARenderer {

    ImageBasedLightProbeGenerator::ImageBasedLightProbeGenerator(size_t probeResolution)
            : mProbeResolution(probeResolution),
              mConversionCubemap(probeResolution),
              mSpecularIrradianceMipCount(5),
              mFramebuffer(probeResolution) {}

    void ImageBasedLightProbeGenerator::convertEquirectangilarMap(const GLFloatTexture2D<GLTexture::Float::RGB16F> &equirectangularMap) {
        mFramebuffer.redirectRenderingToTextures({mConversionCubemap.size()}, GLFramebuffer::UnderlyingBuffer::None, &mConversionCubemap);
        mConversionShader.bind();
        mConversionShader.ensureSamplerValidity([&] {
            mConversionShader.setEquirectangularEnvironmentMap(equirectangularMap);
        });
        Drawable::TriangleStripQuad::Draw();
    }

    void ImageBasedLightProbeGenerator::buildBRDFIntegrationMap() {
        mFramebuffer.redirectRenderingToTextures({mBRDFIntegrationMap->size()}, GLFramebuffer::UnderlyingBuffer::None, mBRDFIntegrationMap);
        mBRDFIntegrationShader.bind();
        Drawable::TriangleStripQuad::Draw();
    }

    void ImageBasedLightProbeGenerator::buildDiffuseIrradianceMap(
            const GLFloatTextureCubemap<GLTexture::Float::RGB16F> &radiance,
            GLFloatTextureCubemap<GLTexture::Float::RGB16F> &irradiance) {

        mFramebuffer.redirectRenderingToTextures({irradiance.size()}, GLFramebuffer::UnderlyingBuffer::None, &irradiance);
        mDiffuseRadianceConvolutionShader.bind();
        mDiffuseRadianceConvolutionShader.ensureSamplerValidity([&] {
            mDiffuseRadianceConvolutionShader.setEnvironmentRadianceMap(radiance);
        });
        Drawable::TriangleStripQuad::Draw();
    }

    void ImageBasedLightProbeGenerator::buildSpecularIrradianceMap(
            const GLFloatTextureCubemap<GLTexture::Float::RGB16F> &radiance,
            GLFloatTextureCubemap<GLTexture::Float::RGB16F> &irradiance) {

        mSpecularRadianceConvolutionShader.bind();
        mSpecularRadianceConvolutionShader.ensureSamplerValidity([&] {
            mSpecularRadianceConvolutionShader.setEnvironmentRadianceMap(radiance);
        });

        irradiance.generateMipMaps();

        for (int mip = 0; mip < mSpecularIrradianceMipCount; ++mip) {
            float roughness = float(mip) / float(mSpecularIrradianceMipCount - 1);
            mSpecularRadianceConvolutionShader.setRoughness(roughness);
            mFramebuffer.redirectRenderingToTexturesMip(mip, GLFramebuffer::UnderlyingBuffer::None, &irradiance);
            Drawable::TriangleStripQuad::Draw();
        }
    }

    ImageBasedLightProbe ImageBasedLightProbeGenerator::generate(const GLFloatTexture2D<GLTexture::Float::RGB16F> &HDREquirectangularMap) {
        convertEquirectangilarMap(HDREquirectangularMap);
        return generate(mConversionCubemap);
    }

    ImageBasedLightProbe ImageBasedLightProbeGenerator::generate(GLFloatTextureCubemap<GLTexture::Float::RGB16F> &HDRCubemap) {
        glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
        glDisable(GL_BLEND);

        if (!mBRDFIntegrationMap) {
            mBRDFIntegrationMap = std::make_shared<GLFloatTexture2D<GLTexture::Float::RG16F>>(512);
            buildBRDFIntegrationMap();
        }

        HDRCubemap.generateMipMaps();

        ImageBasedLightProbe probe(mProbeResolution, 32);
        probe.mBRDFIntegrationMap = mBRDFIntegrationMap;
        probe.mSpecularIrradianceMipCount = mSpecularIrradianceMipCount;
        buildDiffuseIrradianceMap(HDRCubemap, *probe.mDiffuseIrradiance);
        buildSpecularIrradianceMap(HDRCubemap, *probe.mSpecularIrradiance);
        return probe;
    }

}