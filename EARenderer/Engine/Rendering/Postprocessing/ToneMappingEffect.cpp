//
//  ToneMappingEffect.cpp
//  EARenderer
//
//  Created by Pavel Muratov on 6/27/18.
//  Copyright © 2018 MPO. All rights reserved.
//

#include "ToneMappingEffect.hpp"

namespace EARenderer {

#pragma mark - Tone mapping
    
    void ToneMappingEffect::toneMap(std::shared_ptr<const GLFloatTexture2D> inputImage,
                                    std::shared_ptr<GLFloatTexture2D> outputImage,
                                    std::shared_ptr<PostprocessTexturePool> texturePool)
    {
        mToneMappingShader.bind();
        mToneMappingShader.ensureSamplerValidity([&]() {
            mToneMappingShader.setImage(*inputImage);
        });

        texturePool->redirectRenderingToTextures(outputImage);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }

}
