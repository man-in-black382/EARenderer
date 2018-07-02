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
#include "GLFramebuffer.hpp"
#include "GLHDRTexture2D.hpp"
#include "Size2D.hpp"
#include "GLSLGaussianBlur.hpp"
#include "GaussianBlurSettings.hpp"

#include <memory>
#include <vector>

namespace EARenderer {

    class GaussianBlurEffect: public PostprocessEffect {
    private:
        std::shared_ptr<const GLHDRTexture2D> mInputImage;
        std::shared_ptr<GLHDRTexture2D> mFirstOutputImage;
        std::shared_ptr<GLHDRTexture2D> mSecondOutputImage;
        GLSLGaussianBlur mBlurShader;
        std::vector<float> mWeights;
        std::vector<float> mTextureOffsets;
        GaussianBlurSettings mSettings;

        void computeWeightsAndOffsets();

    public:
        GaussianBlurEffect(std::shared_ptr<const GLHDRTexture2D> inputImage,
                           std::shared_ptr<GLFramebuffer> sharedFramebuffer);

        std::shared_ptr<GLHDRTexture2D> outputImage() const;
        std::shared_ptr<GLHDRTexture2D> blur(const GaussianBlurSettings& settings);
    };

}

#endif /* GaussianBlurEffect_hpp */
