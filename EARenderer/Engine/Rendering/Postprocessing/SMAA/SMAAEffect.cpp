//
//  SMAAEffect.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 10.10.2018.
//  Copyright © 2018 MPO. All rights reserved.
//

#include "SMAAEffect.hpp"
#include "Drawable.hpp"
#include "Size2D.hpp"
#include "SMAAAreaTex.h"
#include "SMAASearchTex.h"

#include <array>

namespace EARenderer {

#pragma mark - Lifecycle

    SMAAEffect::SMAAEffect(GLFramebuffer *sharedFramebuffer, PostprocessTexturePool *sharedTexturePool)
            : PostprocessEffect(sharedFramebuffer, sharedTexturePool),
              mAreaTexture(Size2D(AREATEX_WIDTH, AREATEX_HEIGHT), areaTexBytes),
              mSearchTexture(Size2D(SEARCHTEX_WIDTH, SEARCHTEX_HEIGHT), searchTexBytes),
              mEdgesTexture(sharedFramebuffer->size()),
              mBlendTexture(sharedFramebuffer->size()) {
    }

#pragma mark - Antialiasing

    void SMAAEffect::detectEdges(const PostprocessTexturePool::PostprocessTexture &image) {
        mEdgeDetectionShader.bind();
        mEdgeDetectionShader.ensureSamplerValidity([&]() {
            mEdgeDetectionShader.setImage(image);
        });

        mFramebuffer->redirectRenderingToTextures(GLFramebuffer::UnderlyingBuffer::Color, &mEdgesTexture);
        Drawable::TriangleStripQuad::Draw();
    }

    void SMAAEffect::calculateBlendingWeights() {
        mBlendingWeightCalculationShader.bind();
        mBlendingWeightCalculationShader.ensureSamplerValidity([&]() {
            mBlendingWeightCalculationShader.setEdgesTexture(mEdgesTexture);
            mBlendingWeightCalculationShader.setAreaTexture(mAreaTexture);
            mBlendingWeightCalculationShader.setSearchTexture(mSearchTexture);
        });

        mFramebuffer->redirectRenderingToTextures(GLFramebuffer::UnderlyingBuffer::Color, &mBlendTexture);
        Drawable::TriangleStripQuad::Draw();
    }

    void SMAAEffect::blendNeighbors(const PostprocessTexturePool::PostprocessTexture &image, PostprocessTexturePool::PostprocessTexture &outputImage) {
        mNeighborhoodBlendingShader.bind();
        mNeighborhoodBlendingShader.ensureSamplerValidity([&]() {
            mNeighborhoodBlendingShader.setImage(image);
            mNeighborhoodBlendingShader.setBlendingWeights(mBlendTexture);
        });

        mFramebuffer->redirectRenderingToTextures(GLFramebuffer::UnderlyingBuffer::None, &outputImage);
        Drawable::TriangleStripQuad::Draw();
    }

    void SMAAEffect::antialise(const PostprocessTexturePool::PostprocessTexture &inputImage, PostprocessTexturePool::PostprocessTexture &outputImage) {
        detectEdges(inputImage);
        calculateBlendingWeights();
        blendNeighbors(inputImage, outputImage);
    }

}
