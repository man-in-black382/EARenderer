//
//  ScreenSpaceReflectionEffect.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 15.07.2018.
//  Copyright © 2018 MPO. All rights reserved.
//

#include "ScreenSpaceReflectionEffect.hpp"

namespace EARenderer {

    void ScreenSpaceReflectionEffect::applyReflections(const Camera& camera,
                                                      std::shared_ptr<const GLFloatTexture2D<GLTexture::Float::RGBA16F>> inputImage,
                                                      std::shared_ptr<const SceneGBuffer> GBuffer,
                                                      std::shared_ptr<GLFloatTexture2D<GLTexture::Float::RGBA16F>> outputImage,
                                                      std::shared_ptr<PostprocessTexturePool> texturePool)
    {
        mSSRShader.bind();
        mSSRShader.ensureSamplerValidity([&]() {
            mSSRShader.setCamera(camera);
            mSSRShader.setFrame(*inputImage);
            mSSRShader.setGBuffer(*GBuffer);
        });

        texturePool->redirectRenderingToTextures(outputImage);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }

}
