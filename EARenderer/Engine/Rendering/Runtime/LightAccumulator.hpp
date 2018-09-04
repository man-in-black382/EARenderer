//
//  LightAccumulator.hpp
//  EARenderer
//
//  Created by Pavel Muratov on 9/3/18.
//  Copyright © 2018 MPO. All rights reserved.
//

#ifndef LightAccumulator_hpp
#define LightAccumulator_hpp

#include "GLTexture2D.hpp"
#include "SceneGBuffer.hpp"
#include "Scene.hpp"
#include "ShadowMapper.hpp"
#include "GLFramebuffer.hpp"
#include "GLSLDeferredCookTorrance.hpp"

#include <memory>

namespace EARenderer {

    class LightAccumulator {
    private:
        Scene *mScene = nullptr;
        std::shared_ptr<GLFloatTexture2D<GLTexture::Float::RGBA16F>> mLightBuffer;
        std::shared_ptr<const SceneGBuffer> mGBuffer;
        std::shared_ptr<const ShadowMapper> mShadowMapper;
        std::shared_ptr<GLFramebuffer> mFramebuffer;
        GLSLDeferredCookTorrance mCookTorranceShader;
        RenderingSettings mSettings;

        void renderDirectionalLights();
        void renderPointLights();

    public:
        LightAccumulator(Scene *scene,
                         std::shared_ptr<const SceneGBuffer> gBuffer,
                         std::shared_ptr<const ShadowMapper> shadowMapper,
                         std::shared_ptr<GLFramebuffer> framebuffer);

        void setSettings(const RenderingSettings& settings);
        
        void accumulateDirectLighting();
    };

}

#endif /* LightAccumulator_hpp */
