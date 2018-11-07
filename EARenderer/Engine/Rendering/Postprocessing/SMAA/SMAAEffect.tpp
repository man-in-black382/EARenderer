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
    mAreaTexture(Size2D(AREATEX_WIDTH, AREATEX_HEIGHT), areaTexBytes),
    mSearchTexture(Size2D(SEARCHTEX_WIDTH, SEARCHTEX_HEIGHT), searchTexBytes),

    mEdgesTexture(sharedFramebuffer->size()),
    mBlendTexture(sharedFramebuffer->size())
    { }

#pragma mark - Antialiasing

    template <GLTexture::Float TextureFormat>
    void SMAAEffect<TextureFormat>::detectEdges(std::shared_ptr<const typename PostprocessTexturePool<TextureFormat>::PostprocessTexture> image) {
        mEdgeDetectionShader.bind();
        mEdgeDetectionShader.ensureSamplerValidity([&]() {
            mEdgeDetectionShader.setImage(*image);
        });

        this->mFramebuffer->redirectRenderingToTexturesMip(0, GLFramebuffer::UnderlyingBuffer::Color | GLFramebuffer::UnderlyingBuffer::Depth, &mEdgesTexture);
        Drawable::TriangleStripQuad::Draw();
    }

    template <GLTexture::Float TextureFormat>
    void SMAAEffect<TextureFormat>::calculateBlendingWeights() {
        mBlendingWeightCalculationShader.bind();
        mBlendingWeightCalculationShader.ensureSamplerValidity([&]() {
            mBlendingWeightCalculationShader.setEdgesTexture(mEdgesTexture);
            mBlendingWeightCalculationShader.setAreaTexture(mAreaTexture);
            mBlendingWeightCalculationShader.setSearchTexture(mSearchTexture);
        });

        this->mFramebuffer->redirectRenderingToTexturesMip(0, GLFramebuffer::UnderlyingBuffer::Color | GLFramebuffer::UnderlyingBuffer::Depth, &mBlendTexture);
        Drawable::TriangleStripQuad::Draw();
    }

    template <GLTexture::Float TextureFormat>
    void SMAAEffect<TextureFormat>::blendNeighbors(std::shared_ptr<const typename PostprocessTexturePool<TextureFormat>::PostprocessTexture> image,
                                                   std::shared_ptr<typename PostprocessTexturePool<TextureFormat>::PostprocessTexture> outputImage) {
        mNeighborhoodBlendingShader.bind();
        mNeighborhoodBlendingShader.ensureSamplerValidity([&]() {
            mNeighborhoodBlendingShader.setImage(*image);
            mNeighborhoodBlendingShader.setBlendingWeights(mBlendTexture);
        });

        this->mFramebuffer->redirectRenderingToTexturesMip(0, GLFramebuffer::UnderlyingBuffer::Color | GLFramebuffer::UnderlyingBuffer::Depth, outputImage);
        Drawable::TriangleStripQuad::Draw();
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
