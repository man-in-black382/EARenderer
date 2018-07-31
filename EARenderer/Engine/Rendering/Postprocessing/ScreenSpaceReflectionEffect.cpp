//
//  ScreenSpaceReflectionEffect.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 15.07.2018.
//  Copyright © 2018 MPO. All rights reserved.
//

#include "ScreenSpaceReflectionEffect.hpp"

namespace EARenderer {

#pragma mark - Pivate Helpers

    void ScreenSpaceReflectionEffect::renderReflections(const Camera& camera,
                                                        std::shared_ptr<const GLFloatTexture2D<GLTexture::Float::RGBA16F>> lightBuffer,
                                                        std::shared_ptr<const SceneGBuffer> GBuffer,
                                                        std::shared_ptr<GLFloatTexture2D<GLTexture::Float::RGBA16F>> mirrorReflections,
                                                        std::shared_ptr<PostprocessTexturePool> texturePool)
    {
        mSSRShader.bind();
        mSSRShader.ensureSamplerValidity([&]() {
            mSSRShader.setCamera(camera);
            mSSRShader.setFrame(*lightBuffer);
            mSSRShader.setGBuffer(*GBuffer);
        });

        texturePool->redirectRenderingToTextures(mirrorReflections);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }

    void ScreenSpaceReflectionEffect::blurReflections(std::shared_ptr<GLFloatTexture2D<GLTexture::Float::RGBA16F>> mirrorReflections,
                                                      std::shared_ptr<PostprocessTexturePool> texturePool)
    {
        size_t blurRadius = 3;

        for (size_t mipLevel = 0; mipLevel < mirrorReflections->mipMapsCount(); mipLevel++) {
            GaussianBlurSettings blurSettings { blurRadius, blurRadius, mipLevel, mipLevel + 1 };
            mBlurEffect.blur(mirrorReflections, mirrorReflections, texturePool, blurSettings);
        }
    }

    void ScreenSpaceReflectionEffect::performConeTracing(const Camera& camera,
                                                         std::shared_ptr<const GLFloatTexture2D<GLTexture::Float::RGBA16F>> inputImage,
                                                         std::shared_ptr<const SceneGBuffer> GBuffer,
                                                         std::shared_ptr<GLFloatTexture2D<GLTexture::Float::RGBA16F>> outputImage,
                                                         std::shared_ptr<PostprocessTexturePool> texturePool)
    {
        
    }

#pragma mark - Public Interface

    void ScreenSpaceReflectionEffect::applyReflections(const Camera& camera,
                                                      std::shared_ptr<const GLFloatTexture2D<GLTexture::Float::RGBA16F>> lightBuffer,
                                                      std::shared_ptr<const SceneGBuffer> GBuffer,
                                                      std::shared_ptr<GLFloatTexture2D<GLTexture::Float::RGBA16F>> outputImage,
                                                      std::shared_ptr<PostprocessTexturePool> texturePool)
    {
        auto mirrorReflections = outputImage;//texturePool->claim();
        renderReflections(camera, lightBuffer, GBuffer, mirrorReflections, texturePool);
//        blurReflections(mirrorReflections, texturePool);
    }

}
