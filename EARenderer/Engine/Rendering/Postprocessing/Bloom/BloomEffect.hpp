//
//  BloomFilter.hpp
//  EARenderer
//
//  Created by Pavel Muratov on 6/26/18.
//  Copyright © 2018 MPO. All rights reserved.
//

#ifndef BloomFilter_hpp
#define BloomFilter_hpp

#include "PostprocessEffect.hpp"
#include "GLTexture2D.hpp"
#include "GaussianBlurEffect.hpp"
#include "GLFramebuffer.hpp"
#include "GLSLBloom.hpp"
#include "BloomSettings.hpp"

#include <memory>

namespace EARenderer {

    template <GLTexture::Float TextureFormat>
    class BloomEffect: public PostprocessEffect<TextureFormat> {
    private:
        GaussianBlurEffect<TextureFormat> mSmallBlurEffect;
        GaussianBlurEffect<TextureFormat> mMediumBlurEffect;
        GaussianBlurEffect<TextureFormat> mLargeBlurEffect;

        GLSLBloom mBloomShader;

    public:
        BloomEffect(std::shared_ptr<GLFramebuffer> sharedFramebuffer, std::shared_ptr<PostprocessTexturePool<TextureFormat>> sharedTexturePool);

        void bloom(std::shared_ptr<const typename PostprocessTexturePool<TextureFormat>::PostprocessTexture> baseImage,
                   std::shared_ptr<typename PostprocessTexturePool<TextureFormat>::PostprocessTexture> thresholdFilteredImage,
                   std::shared_ptr<typename PostprocessTexturePool<TextureFormat>::PostprocessTexture> outputImage,
                   const BloomSettings& settings);
    };

}

#include "BloomEffect.tpp"

#endif /* BloomFilter_hpp */
