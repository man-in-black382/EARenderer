//
//  SMAAEffect.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 10.10.2018.
//  Copyright © 2018 MPO. All rights reserved.
//

#include "Size2D.hpp"
#include "SMAAAreaTex.h"
#include "SMAASearchTex.h"

#include <array>

namespace EARenderer {

#pragma mark - Lifecycle

    template <GLTexture::Float TextureFormat>
    SMAAEffect<TextureFormat>::SMAAEffect(std::shared_ptr<GLFramebuffer> sharedFramebuffer, std::shared_ptr<PostprocessTexturePool<TextureFormat>> sharedTexturePool)
    :
    PostprocessEffect<TextureFormat>(sharedFramebuffer, sharedTexturePool),
//    mAreaTexture(Size2D(AREATEX_WIDTH, AREATEX_HEIGHT), areaTextureFlipped().data()),
//    mSearchTexture(Size2D(SEARCHTEX_WIDTH, SEARCHTEX_HEIGHT), searchTextureFlipped().data()),
    mAreaTexture(Size2D(AREATEX_WIDTH, AREATEX_HEIGHT), areaTexBytes),
    mSearchTexture(Size2D(SEARCHTEX_WIDTH, SEARCHTEX_HEIGHT), searchTexBytes),

    mEdgesTexture(sharedFramebuffer->size()),
    mBlendTexture(sharedFramebuffer->size())
    { }

#pragma mark - Helpers

    template <GLTexture::Float TextureFormat>
    std::array<unsigned char, AREATEX_SIZE> SMAAEffect<TextureFormat>::areaTextureFlipped() const {
        std::array<unsigned char, AREATEX_SIZE> texels;
        std::array<unsigned char, AREATEX_PITCH> tempBuffer;

        std::copy(std::begin(areaTexBytes), std::end(areaTexBytes), std::begin(texels));
        
        for (size_t i = 0; i < AREATEX_HEIGHT; i++) {
            // Copy a line to temporary buffer
            std::copy(std::begin(texels) + (i * AREATEX_PITCH),
                      std::begin(texels) + ((i + 1) * AREATEX_PITCH),
                      std::begin(tempBuffer));

            // Swap last line with first line
            std::copy(std::begin(texels) + ((AREATEX_HEIGHT - i - 2) * AREATEX_PITCH),
                      std::begin(texels) + ((AREATEX_HEIGHT - i - 1) * AREATEX_PITCH),
                      std::begin(texels) + (i * AREATEX_PITCH));

            // Swap temporary line with last line
            std::copy(std::begin(tempBuffer),
                      std::end(tempBuffer),
                      std::begin(texels) + ((AREATEX_HEIGHT - i - 1) * AREATEX_PITCH));
        }

        return texels;
    }

    template <GLTexture::Float TextureFormat>
    std::array<unsigned char, SEARCHTEX_SIZE> SMAAEffect<TextureFormat>::searchTextureFlipped() const {
        std::array<unsigned char, SEARCHTEX_SIZE> texels;
        std::array<unsigned char, SEARCHTEX_PITCH> tempBuffer;

        std::copy(std::begin(searchTexBytes), std::end(searchTexBytes), std::begin(texels));

        for (size_t i = 0; i < SEARCHTEX_HEIGHT; i++) {
            // Copy a line to temporary buffer
            std::copy(std::begin(texels) + (i * SEARCHTEX_PITCH),
                      std::begin(texels) + ((i + 1) * SEARCHTEX_PITCH),
                      std::begin(tempBuffer));

            // Swap last line with first line
            std::copy(std::begin(texels) + ((SEARCHTEX_HEIGHT - i - 2) * SEARCHTEX_PITCH),
                      std::begin(texels) + ((SEARCHTEX_HEIGHT - i - 1) * SEARCHTEX_PITCH),
                      std::begin(texels) + (i * SEARCHTEX_PITCH));

            // Swap temporary line with last line
            std::copy(std::begin(tempBuffer),
                      std::end(tempBuffer),
                      std::begin(texels) + ((SEARCHTEX_HEIGHT - i - 1) * SEARCHTEX_PITCH));
        }

        return texels;
    }

#pragma mark - Antialiasing

    template <GLTexture::Float TextureFormat>
    void SMAAEffect<TextureFormat>::detectEdges(std::shared_ptr<const typename PostprocessTexturePool<TextureFormat>::PostprocessTexture> image) {
        mEdgeDetectionShader.bind();
        mEdgeDetectionShader.ensureSamplerValidity([&]() {
            mEdgeDetectionShader.setImage(*image);
        });

        this->mFramebuffer->redirectRenderingToTexturesMip(0, &mEdgesTexture);
        TriangleStripQuad::Draw();
    }

    template <GLTexture::Float TextureFormat>
    void SMAAEffect<TextureFormat>::calculateBlendingWeights() {
        mBlendingWeightCalculationShader.bind();
        mBlendingWeightCalculationShader.ensureSamplerValidity([&]() {
            mBlendingWeightCalculationShader.setEdgesTexture(mEdgesTexture);
            mBlendingWeightCalculationShader.setAreaTexture(mAreaTexture);
            mBlendingWeightCalculationShader.setSearchTexture(mSearchTexture);
        });

        this->mFramebuffer->redirectRenderingToTexturesMip(0, &mBlendTexture);
        TriangleStripQuad::Draw();
    }

    template <GLTexture::Float TextureFormat>
    void SMAAEffect<TextureFormat>::blendNeighbors(std::shared_ptr<const typename PostprocessTexturePool<TextureFormat>::PostprocessTexture> image,
                                                   std::shared_ptr<typename PostprocessTexturePool<TextureFormat>::PostprocessTexture> outputImage) {
        mNeighborhoodBlendingShader.bind();
        mNeighborhoodBlendingShader.ensureSamplerValidity([&]() {
            mNeighborhoodBlendingShader.setImage(*image);
            mNeighborhoodBlendingShader.setBlendingWeights(mBlendTexture);
        });

        this->mFramebuffer->redirectRenderingToTexturesMip(0, outputImage);
        TriangleStripQuad::Draw();
    }

    template <GLTexture::Float TextureFormat>
    void SMAAEffect<TextureFormat>::antialise(std::shared_ptr<const typename PostprocessTexturePool<TextureFormat>::PostprocessTexture> inputImage,
                                              std::shared_ptr<typename PostprocessTexturePool<TextureFormat>::PostprocessTexture> outputImage)
    {
        detectEdges(inputImage);
        calculateBlendingWeights();
        blendNeighbors(inputImage, outputImage);
    }

}
