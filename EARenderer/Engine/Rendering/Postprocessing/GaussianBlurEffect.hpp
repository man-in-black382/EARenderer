//
//  GaussianBlurEffect.hpp
//  EARenderer
//
//  Created by Pavel Muratov on 6/14/18.
//  Copyright © 2018 MPO. All rights reserved.
//

#ifndef GaussianBlurEffect_hpp
#define GaussianBlurEffect_hpp

#include "PostprocessTexturePool.hpp"
#include "GLFramebuffer.hpp"
#include "GLTexture2D.hpp"
#include "Size2D.hpp"
#include "GLSLGaussianBlur.hpp"
#include "GaussianBlurSettings.hpp"

#include <memory>
#include <vector>

namespace EARenderer {

    class GaussianBlurEffect {
    private:
        GLSLGaussianBlur mBlurShader;
        std::vector<float> mWeights;
        std::vector<float> mTextureOffsets;
        GaussianBlurSettings mSettings;

        void computeWeightsAndOffsets();

    public:
        GaussianBlurEffect();

        void blur(std::shared_ptr<const GLFloatTexture2D<GLTexture::Float::RGBA16F>> inputImage,
                  std::shared_ptr<GLFloatTexture2D<GLTexture::Float::RGBA16F>> outputImage,
                  std::shared_ptr<PostprocessTexturePool> texturePool,
                  const GaussianBlurSettings& settings);
    };

}

#endif /* GaussianBlurEffect_hpp */
