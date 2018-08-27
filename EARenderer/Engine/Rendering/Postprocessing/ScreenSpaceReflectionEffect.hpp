//
//  ScreenSpaceReflectionEffect.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 15.07.2018.
//  Copyright © 2018 MPO. All rights reserved.
//

#ifndef ScreenSpaceReflectionEffect_hpp
#define ScreenSpaceReflectionEffect_hpp

#include "PostprocessEffect.hpp"
#include "GLSLScreenSpaceReflections.hpp"
#include "GLSLConeTracing.hpp"
#include "PostprocessTexturePool.hpp"
#include "SceneGBuffer.hpp"
#include "Camera.hpp"
#include "GaussianBlurEffect.hpp"

#include <memory>

namespace EARenderer {

    template <GLTexture::Float TextureFormat>
    class ScreenSpaceReflectionEffect: public PostprocessEffect<TextureFormat> {
    private:
        GLSLScreenSpaceReflections mSSRShader;
        GLSLConeTracing mConeTracingShader;
        GaussianBlurEffect<TextureFormat> mBlurEffect;
        
        void traceReflections(const Camera& camera,
                              std::shared_ptr<const SceneGBuffer> GBuffer,
                              std::shared_ptr<typename PostprocessTexturePool<TextureFormat>::PostprocessTexture> rayHitInfo);

        void blurProgressively(std::shared_ptr<typename PostprocessTexturePool<TextureFormat>::PostprocessTexture> mirrorReflections);

        void traceCones(std::shared_ptr<const typename PostprocessTexturePool<TextureFormat>::PostprocessTexture> lightBuffer,
                        std::shared_ptr<const typename PostprocessTexturePool<TextureFormat>::PostprocessTexture> rayHitInfo,
                        std::shared_ptr<const SceneGBuffer> GBuffer,
                        std::shared_ptr<typename PostprocessTexturePool<TextureFormat>::PostprocessTexture> outputImage);

    public:
        ScreenSpaceReflectionEffect(std::shared_ptr<GLFramebuffer> sharedFramebuffer,
                                    std::shared_ptr<PostprocessTexturePool<TextureFormat>> sharedTexturePool);

        void applyReflections(const Camera& camera,
                              std::shared_ptr<const SceneGBuffer> GBuffer,
                              std::shared_ptr<typename PostprocessTexturePool<TextureFormat>::PostprocessTexture> lightBuffer,
                              std::shared_ptr<typename PostprocessTexturePool<TextureFormat>::PostprocessTexture> outputImage);
    };

}

#include "ScreenSpaceReflectionEffect.tpp"

#endif /* ScreenSpaceReflectionEffect_hpp */
