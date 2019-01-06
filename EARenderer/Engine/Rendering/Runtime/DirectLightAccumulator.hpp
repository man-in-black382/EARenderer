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
#include "GLSampler.hpp"
#include "SceneGBuffer.hpp"
#include "Scene.hpp"
#include "ShadowMapper.hpp"
#include "GLFramebuffer.hpp"
#include "GLSLDirectLightEvaluation.hpp"

#include <memory>

namespace EARenderer {

    class DirectLightAccumulator {
    private:
        const Scene *mScene;
        const SceneGBuffer *mGBuffer;
        const ShadowMapper *mShadowMapper;

        GLSLDirectLightEvaluation mLightEvaluationShader;
        RenderingSettings mSettings;

        void renderDirectionalLights();

        void renderPointLights();

    public:
        DirectLightAccumulator(const Scene *scene, const SceneGBuffer *gBuffer, const ShadowMapper *shadowMapper);

        void setRenderingSettings(const RenderingSettings &settings);

        void render();
    };

}

#endif /* LightAccumulator_hpp */
