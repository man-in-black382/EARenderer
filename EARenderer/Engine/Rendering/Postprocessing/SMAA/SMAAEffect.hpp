//
//  SMAAEffect.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 10.10.2018.
//  Copyright © 2018 MPO. All rights reserved.
//

#ifndef SMAAEffect_hpp
#define SMAAEffect_hpp

#include "PostprocessEffect.hpp"
#include "GLTexture2D.hpp"
#include "GLSLSMAAEdgeDetection.hpp"

namespace EARenderer {

    template <GLTexture::Float TextureFormat>
    class SMAAEffect: public PostprocessEffect<TextureFormat> {
    private:
        GLNormalizedTexture2D<GLTexture::Normalized::RG> mAreaTexture;
        GLNormalizedTexture2D<GLTexture::Normalized::R> mSearchTexture;
        GLFloatTexture2D<GLTexture::Float::RG16F> mEdgesTexture;
        GLFloatTexture2D<GLTexture::Float::RGBA16F> mBlendTexture;
        GLSLSMAAEdgeDetection mEdgeDetectionShader;

    public:
        SMAAEffect(std::shared_ptr<GLFramebuffer> sharedFramebuffer, std::shared_ptr<PostprocessTexturePool<TextureFormat>> sharedTexturePool);

        void antialise(std::shared_ptr<const typename PostprocessTexturePool<TextureFormat>::PostprocessTexture> inputImage,
                       std::shared_ptr<typename PostprocessTexturePool<TextureFormat>::PostprocessTexture> outputImage);
    };

}

#include "SMAAEffect.tpp"

#endif /* SMAAEffect_hpp */
