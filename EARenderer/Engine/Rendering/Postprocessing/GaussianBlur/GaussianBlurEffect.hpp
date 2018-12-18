//
//  GaussianBlurEffect.hpp
//  EARenderer
//
//  Created by Pavel Muratov on 6/14/18.
//  Copyright © 2018 MPO. All rights reserved.
//

#ifndef GaussianBlurEffect_hpp
#define GaussianBlurEffect_hpp

#include "PostprocessEffect.hpp"
#include "GLTexture2D.hpp"
#include "Size2D.hpp"
#include "GLSLGaussianBlur.hpp"
#include "GaussianBlurSettings.hpp"

#include <memory>
#include <vector>

namespace EARenderer {

    template<GLTexture::Float TextureFormat>
    class GaussianBlurEffect : public PostprocessEffect<TextureFormat> {
    private:
        GLSLGaussianBlur mBlurShader;
        std::vector<float> mWeights;
        std::vector<float> mTextureOffsets;
        GaussianBlurSettings mSettings;

        void computeWeightsAndOffsetsIfNeeded(const GaussianBlurSettings &settings);

    public:
        using PostprocessEffect<TextureFormat>::PostprocessEffect;

        void blur(std::shared_ptr<const typename PostprocessTexturePool<TextureFormat>::PostprocessTexture> inputImage,
                std::shared_ptr<typename PostprocessTexturePool<TextureFormat>::PostprocessTexture> outputImage,
                const GaussianBlurSettings &settings);
    };

}

#include "GaussianBlurEffect.tpp"

#endif /* GaussianBlurEffect_hpp */
