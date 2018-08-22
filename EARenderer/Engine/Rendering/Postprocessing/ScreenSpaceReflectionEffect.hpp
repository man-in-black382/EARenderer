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

    class ScreenSpaceReflectionEffect: public PostprocessEffect {
    private:
        GLSLScreenSpaceReflections mSSRShader;
        GLSLConeTracing mConeTracingShader;
        GaussianBlurEffect mBlurEffect;
        
        void traceReflections(const Camera& camera,
                              std::shared_ptr<const SceneGBuffer> GBuffer,
                              std::shared_ptr<PostprocessTexturePool::PostprocessTexture> rayHitInfo);

        void blurProgressively(std::shared_ptr<PostprocessTexturePool::PostprocessTexture> mirrorReflections);

        void traceCones(std::shared_ptr<const PostprocessTexturePool::PostprocessTexture> lightBuffer,
                        std::shared_ptr<const PostprocessTexturePool::PostprocessTexture> rayHitInfo,
                        std::shared_ptr<const SceneGBuffer> GBuffer,
                        std::shared_ptr<PostprocessTexturePool::PostprocessTexture> outputImage);

    public:
        ScreenSpaceReflectionEffect(std::shared_ptr<GLFramebuffer> sharedFramebuffer, std::shared_ptr<PostprocessTexturePool> sharedTexturePool);

        void applyReflections(const Camera& camera,
                              std::shared_ptr<const SceneGBuffer> GBuffer,
                              std::shared_ptr<PostprocessTexturePool::PostprocessTexture> lightBuffer,
                              std::shared_ptr<PostprocessTexturePool::PostprocessTexture> outputImage);
    };

}

#endif /* ScreenSpaceReflectionEffect_hpp */
