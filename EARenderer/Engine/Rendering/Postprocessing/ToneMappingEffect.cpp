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
    
    void ToneMappingEffect::toneMap(std::shared_ptr<const PostprocessTexturePool::PostprocessTexture> inputImage,
                                    std::shared_ptr<PostprocessTexturePool::PostprocessTexture> outputImage,
                                    std::shared_ptr<PostprocessTexturePool> texturePool)
    {
        mToneMappingShader.bind();
        mToneMappingShader.ensureSamplerValidity([&]() {
            mToneMappingShader.setImage(*inputImage);
        });

        texturePool->redirectRenderingToTexturesMip(0, outputImage);
        TriangleStripQuad::Draw();
    }

}
