//
//  ToneMappingEffect.cpp
//  EARenderer
//
//  Created by Pavel Muratov on 6/27/18.
//  Copyright © 2018 MPO. All rights reserved.
//

#include "ToneMappingEffect.hpp"
#include "Drawable.hpp"

namespace EARenderer {

#pragma mark - Tone mapping

    void ToneMappingEffect::computeAverageLuminance(std::shared_ptr<GLFloatTexture2D<GLTexture::Float::R16F>> luminance) {
        // For now, just average luminance values through automatic mipmap generation
        luminance->generateMipMaps();
    }
    
    void ToneMappingEffect::toneMap(std::shared_ptr<const PostprocessTexturePool::PostprocessTexture> inputImage,
                                    std::shared_ptr<GLFloatTexture2D<GLTexture::Float::R16F>> luminance,
                                    std::shared_ptr<PostprocessTexturePool::PostprocessTexture> outputImage,
                                    std::shared_ptr<PostprocessTexturePool> texturePool)
    {
        computeAverageLuminance(luminance);

        mToneMappingShader.bind();
        mToneMappingShader.ensureSamplerValidity([&]() {
            mToneMappingShader.setImage(*inputImage);
            mToneMappingShader.setLuminance(*luminance);
        });

        texturePool->redirectRenderingToTexturesMip(0, outputImage);
        TriangleStripQuad::Draw();
    }

}
