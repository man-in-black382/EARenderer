//
//  ToneMappingEffect.hpp
//  EARenderer
//
//  Created by Pavel Muratov on 6/27/18.
//  Copyright © 2018 MPO. All rights reserved.
//

#ifndef ToneMappingEffect_hpp
#define ToneMappingEffect_hpp

#include "GLHDRTexture2D.hpp"
#include "GaussianBlurEffect.hpp"
#include "GLFramebuffer.hpp"
#include "GLSLToneMapping.hpp"

#include <memory>

namespace EARenderer {

    class ToneMappingEffect {
    private:
        std::shared_ptr<const GLHDRTexture2D> mInputImage;
        std::shared_ptr<GLHDRTexture2D> mOutputImage;
        GLFramebuffer mFramebuffer;
        GLSLToneMapping mToneMappingShader;

    public:
        ToneMappingEffect(std::shared_ptr<const GLHDRTexture2D> inputImage);

        std::shared_ptr<GLHDRTexture2D> outputImage() const;
        std::shared_ptr<GLHDRTexture2D> toneMap();
    };

}

#endif /* ToneMappingEffect_hpp */
