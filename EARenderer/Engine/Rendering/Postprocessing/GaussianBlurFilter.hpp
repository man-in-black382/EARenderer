//
//  GaussianBlurEffect.hpp
//  EARenderer
//
//  Created by Pavel Muratov on 6/14/18.
//  Copyright © 2018 MPO. All rights reserved.
//

#ifndef GaussianBlurEffect_hpp
#define GaussianBlurEffect_hpp

#include "GLFramebuffer.hpp"
#include "GLHDRTexture2D.hpp"
#include "Size2D.hpp"
#include "GLSLGaussianBlur.hpp"

#include <memory>
#include <vector>

namespace EARenderer {

    class GaussianBlurFilter {
    private:
        std::shared_ptr<const GLHDRTexture2D> mInputImage;
        std::shared_ptr<GLHDRTexture2D> mFirstOutputImage;
        std::shared_ptr<GLHDRTexture2D> mSecondOutputImage;
        GLFramebuffer mFirstFramebuffer;
        GLFramebuffer mSecondFramebuffer;
        GLSLGaussianBlur mBlurShader;
        std::vector<float> mWeights;
        std::vector<float> mTextureOffsets;
        size_t mBlurRadius = 0;

        void computeWeightsAndOffsets();

    public:
        GaussianBlurFilter(std::shared_ptr<const GLHDRTexture2D> inputImage);

        std::shared_ptr<GLHDRTexture2D> outputImage() const;
        std::shared_ptr<GLHDRTexture2D> blur(size_t radius);
    };

}

#endif /* GaussianBlurEffect_hpp */
