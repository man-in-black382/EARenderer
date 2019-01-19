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
        const Scene *mScene;
        const GPUResourceController *mGPUResourceController;
        const SceneGBuffer *mGBuffer;
        const SurfelData *mSurfelData;
        const DiffuseLightProbeData *mProbeData;
        const ShadowMapper *mShadowMapper;

        RenderingSettings mSettings;

        GLSLSurfelLighting mSurfelLightingShader;
        GLSLSurfelClusterAveraging mSurfelClusterAveragingShader;
        GLSLGridLightProbesUpdate mGridProbesUpdateShader;
        GLSLIndirectLightEvaluation mLightEvaluationShader;

        GLFramebuffer mFramebuffer;
        std::array<GLLDRTexture3D, 4> mGridProbeSHMaps;
        GLFloatTexture2D<GLTexture::Float::R16F> mSurfelsLuminanceMap;
        GLFloatTexture2D<GLTexture::Float::R16F> mSurfelClustersLuminanceMap;

        Size2D framebufferResolution();

        std::array<GLLDRTexture3D, 4> gridProbeSHMaps();

        void relightSurfels();

        void averageSurfelClusterLuminances();

        void updateGridProbes();

    public:
        IndirectLightAccumulator(
                const Scene *scene,
                const GPUResourceController *gpuResourceController,
                const SceneGBuffer *gBuffer,
                const SurfelData *surfelData,
                const DiffuseLightProbeData *probeData,
                const ShadowMapper *shadowMapper
        );

        void setRenderingSettings(const RenderingSettings &settings);

        const std::array<GLLDRTexture3D, 4> &gridProbesSphericalHarmonics() const;

        const GLFloatTexture2D<GLTexture::Float::R16F> &surfelsLuminanceMap() const;

        const GLFloatTexture2D<GLTexture::Float::R16F> &surfelClustersLuminanceMap() const;

        void updateProbes();

        void render();
    };

}

#endif /* IndirectLightAccumulator_hpp */
