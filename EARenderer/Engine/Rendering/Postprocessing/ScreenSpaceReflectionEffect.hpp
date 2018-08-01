//
//  ScreenSpaceReflectionEffect.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 15.07.2018.
//  Copyright © 2018 MPO. All rights reserved.
//

#ifndef ScreenSpaceReflectionEffect_hpp
#define ScreenSpaceReflectionEffect_hpp

#include "GLSLScreenSpaceReflections.hpp"
#include "GLSLConeTracing.hpp"
#include "PostprocessTexturePool.hpp"
#include "SceneGBuffer.hpp"
#include "Camera.hpp"
#include "GaussianBlurEffect.hpp"

#include <memory>

namespace EARenderer {

    class ScreenSpaceReflectionEffect {
    private:
        GLSLScreenSpaceReflections mSSRShader;
        GLSLConeTracing mConeTracingShader;
        GaussianBlurEffect mBlurEffect;
        
        void traceReflections(const Camera& camera,
                              std::shared_ptr<const PostprocessTexturePool::PostprocessTexture> lightBuffer,
                              std::shared_ptr<const SceneGBuffer> GBuffer,
                              std::shared_ptr<PostprocessTexturePool::PostprocessTexture> mirrorReflections,
                              std::shared_ptr<PostprocessTexturePool::PostprocessTexture> rayHitInfo,
                              std::shared_ptr<PostprocessTexturePool> texturePool);

        void blurReflections(std::shared_ptr<PostprocessTexturePool::PostprocessTexture> mirrorReflections,
                             std::shared_ptr<PostprocessTexturePool> texturePool);

        void traceCones(std::shared_ptr<const PostprocessTexturePool::PostprocessTexture> reflections,
                        std::shared_ptr<const PostprocessTexturePool::PostprocessTexture> rayHitInfo,
                        std::shared_ptr<const SceneGBuffer> GBuffer,
                        std::shared_ptr<PostprocessTexturePool::PostprocessTexture> outputImage,
                        std::shared_ptr<PostprocessTexturePool> texturePool);

    public:
        void applyReflections(const Camera& camera,
                              std::shared_ptr<const PostprocessTexturePool::PostprocessTexture> lightBuffer,
                              std::shared_ptr<const SceneGBuffer> GBuffer,
                              std::shared_ptr<PostprocessTexturePool::PostprocessTexture> outputImage,
                              std::shared_ptr<PostprocessTexturePool> texturePool);
    };

}

#endif /* ScreenSpaceReflectionEffect_hpp */
