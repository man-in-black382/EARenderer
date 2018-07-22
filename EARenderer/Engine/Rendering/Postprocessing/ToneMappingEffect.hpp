//
//  ToneMappingEffect.hpp
//  EARenderer
//
//  Created by Pavel Muratov on 6/27/18.
//  Copyright © 2018 MPO. All rights reserved.
//

#ifndef ToneMappingEffect_hpp
#define ToneMappingEffect_hpp

#include "GLTexture2D.hpp"
#include "GaussianBlurEffect.hpp"
#include "GLFramebuffer.hpp"
#include "GLSLToneMapping.hpp"
#include "PostprocessTexturePool.hpp"

#include <memory>

namespace EARenderer {

    class ToneMappingEffect {
    private:
        GLSLToneMapping mToneMappingShader;

    public:
        void toneMap(std::shared_ptr<const GLFloatTexture2D<GLTexture::Float::RGBA16F>> inputImage,
                     std::shared_ptr<GLFloatTexture2D<GLTexture::Float::RGBA16F>> outputImage,
                     std::shared_ptr<PostprocessTexturePool> texturePool);
    };

}

#endif /* ToneMappingEffect_hpp */
