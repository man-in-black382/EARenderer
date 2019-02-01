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
#include "ImageBasedLightProbe.hpp"
#include "RenderingSettings.hpp"

#include <memory>

namespace EARenderer {

    class ScreenSpaceReflectionEffect : public PostprocessEffect {
    private:
        GLSLScreenSpaceReflections mSSRShader;
        GLSLConeTracing mConeTracingShader;
        GaussianBlurEffect mBlurEffect;

        void traceReflections(
                const Camera &camera,
                const SceneGBuffer &GBuffer,
                PostprocessTexturePool::PostprocessTexture &rayHitInfo
        );

        void blurProgressively(PostprocessTexturePool::PostprocessTexture &mirrorReflections);

        void traceCones(
                const Camera &camera,
                const PostprocessTexturePool::PostprocessTexture &lightBuffer,
                const PostprocessTexturePool::PostprocessTexture &rayHitInfo,
                const SceneGBuffer &GBuffer,
                const ImageBasedLightProbe *IBLProbe,
                PostprocessTexturePool::PostprocessTexture &baseOutputImage,
                PostprocessTexturePool::PostprocessTexture &brightOutputImage
        );

    public:
        ScreenSpaceReflectionEffect(GLFramebuffer *sharedFramebuffer, PostprocessTexturePool *sharedTexturePool);

        void applyReflections(
                const Camera &camera,
                const SceneGBuffer &GBuffer,
                const ImageBasedLightProbe *IBLProbe,
                PostprocessTexturePool::PostprocessTexture &lightBuffer,
                PostprocessTexturePool::PostprocessTexture &baseOutputImage,
                PostprocessTexturePool::PostprocessTexture &brightOutputImage
        );
    };

}

#endif /* ScreenSpaceReflectionEffect_hpp */
