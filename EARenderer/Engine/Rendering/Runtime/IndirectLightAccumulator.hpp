//
//  IndirectLightAccumulator.hpp
//  EARenderer
//
//  Created by Pavel Muratov on 9/5/18.
//  Copyright © 2018 MPO. All rights reserved.
//

#ifndef IndirectLightAccumulator_hpp
#define IndirectLightAccumulator_hpp

#include "Scene.hpp"
#include "SurfelData.hpp"
#include "DiffuseLightProbeData.hpp"
#include "ShadowMapper.hpp"
#include "RenderingSettings.hpp"
#include "SceneGBuffer.hpp"
#include "GLFramebuffer.hpp"
#include "GLTexture2D.hpp"
#include "GLLDRTexture3D.hpp"
#include "GLSLSurfelLighting.hpp"
#include "GLSLSurfelClusterAveraging.hpp"
#include "GLSLGridLightProbesUpdate.hpp"
#include "GLSLIndirectLightEvaluation.hpp"

namespace EARenderer {

    class IndirectLightAccumulator {
    private:
        const Scene *mScene = nullptr;
        RenderingSettings mSettings;

        std::shared_ptr<const SceneGBuffer> mGBuffer;
        std::shared_ptr<const SurfelData> mSurfelData;
        std::shared_ptr<const DiffuseLightProbeData> mProbeData;
        std::shared_ptr<const ShadowMapper> mShadowMapper;

        GLSLSurfelLighting mSurfelLightingShader;
        GLSLSurfelClusterAveraging mSurfelClusterAveragingShader;
        GLSLGridLightProbesUpdate mGridProbesUpdateShader;
        GLSLIndirectLightEvaluation mLightEvaluationShader;

        GLFramebuffer mFramebuffer;
        std::shared_ptr<std::array<GLLDRTexture3D, 4>> mGridProbeSHMaps;
        std::shared_ptr<GLFloatTexture2D<GLTexture::Float::R16F>> mSurfelsLuminanceMap;
        std::shared_ptr<GLFloatTexture2D<GLTexture::Float::R16F>> mSurfelClustersLuminanceMap;

        Size2D framebufferResolution();
        std::shared_ptr<std::array<GLLDRTexture3D, 4>> gridProbeSHMaps();

        void relightSurfels();
        void averageSurfelClusterLuminances();
        void updateGridProbes();

    public:
        IndirectLightAccumulator(const Scene *scene,
                                 std::shared_ptr<const SceneGBuffer> gBuffer,
                                 std::shared_ptr<const SurfelData> surfelData,
                                 std::shared_ptr<const DiffuseLightProbeData> probeData,
                                 std::shared_ptr<const ShadowMapper> shadowMapper);

        void setRenderingSettings(const RenderingSettings& settings);

        std::shared_ptr<std::array<GLLDRTexture3D, 4>> gridProbesSphericalHarmonics() const;
        std::shared_ptr<GLFloatTexture2D<GLTexture::Float::R16F>> surfelsLuminanceMap() const;
        std::shared_ptr<GLFloatTexture2D<GLTexture::Float::R16F>> surfelClustersLuminanceMap() const;

        void updateProbes();
        void render();
    };

}

#endif /* IndirectLightAccumulator_hpp */
