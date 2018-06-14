//
//  Renderer.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 29.03.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#ifndef Renderer_hpp
#define Renderer_hpp

#include <unordered_set>
#include <array>
#include <memory>

#include <glm/vec2.hpp>

#include "Scene.hpp"
#include "GLFramebuffer.hpp"
#include "DefaultRenderComponentsProviding.hpp"
#include "FrustumCascades.hpp"
#include "Ray3D.hpp"
#include "SurfelData.hpp"
#include "DiffuseLightProbeData.hpp"
#include "RenderingSettings.hpp"
#include "GaussianBlurEffect.hpp"

#include "GLSLDepthPrepass.hpp"
#include "GLSLCookTorrance.hpp"
#include "GLSLFullScreenQuad.hpp"
#include "GLSLDirectionalDepth.hpp"
#include "GLSLOmnidirectionalDepth.hpp"
#include "GLSLSkybox.hpp"
#include "GLSLGenericGeometry.hpp"
#include "GLSLEquirectangularMapConversion.hpp"
#include "GLSLRadianceConvolution.hpp"
#include "GLSLBRDFIntegration.hpp"
#include "GLSLSurfelLighting.hpp"
#include "GLSLSurfelClusterAveraging.hpp"
#include "GLSLGridLightProbesUpdate.hpp"
#include "GLSLGridLightProbeRendering.hpp"
#include "GLSLLightProbeLinksRendering.hpp"
#include "GLSLProbeOcclusionRendering.hpp"

#include "GLDepthTexture2D.hpp"
#include "GLDepthTextureCubemap.hpp"
#include "GLHDRTexture2DArray.hpp"
#include "GLDepthTexture2DArray.hpp"
#include "GLHDRTexture2D.hpp"
#include "GLHDRTextureCubemap.hpp"
#include "GLTexture2DArray.hpp"
#include "GLLDRTexture3D.hpp"
#include "GLHDRTexture3D.hpp"
#include "GLBufferTexture.hpp"

namespace EARenderer {
    
    class SceneRenderer {
    private:
        uint8_t mNumberOfCascades = 1;
        uint8_t mNumberOfIrradianceMips = 5;
        glm::ivec3 mProbeGridResolution;
        
        const Scene *mScene = nullptr;

        RenderingSettings mSettings;

        std::shared_ptr<const SurfelData> mSurfelData;
        std::shared_ptr<const DiffuseLightProbeData> mDiffuseProbeData;

        DefaultRenderComponentsProviding *mDefaultRenderComponentsProvider = nullptr;
        GLVertexArray<DiffuseLightProbe> mDiffuseProbesVAO;
        FrustumCascades mShadowCascades;

        GLSLDepthPrepass mDepthPrepassShader;
        GLSLDirectionalDepth mDirectionalDepthShader;
        GLSLOmnidirectionalDepth mOmnidirectionalDepthShader;
        GLSLSkybox mSkyboxShader;
        GLSLCookTorrance mCookTorranceShader;
        GLSLEquirectangularMapConversion mEqurectangularMapConversionShader;
        GLSLRadianceConvolution mRadianceConvolutionShader;
        GLSLBRDFIntegration mBRDFIntegrationShader;
        GLSLSurfelLighting mSurfelLightingShader;
        GLSLSurfelClusterAveraging mSurfelClusterAveragingShader;
        GLSLGridLightProbesUpdate mGridProbesUpdateShader;

        GLSLLightProbeLinksRendering mLightProbeLinksRenderingShader;
        GLSLProbeOcclusionRendering mProbeOcclusionRenderingShader;
        
        GLHDRTextureCubemap mEnvironmentMapCube;
        GLHDRTextureCubemap mDiffuseIrradianceMap;
        GLHDRTextureCubemap mSpecularIrradianceMap;
        GLHDRTexture2D mBRDFIntegrationMap;
        GLFramebuffer mIBLFramebuffer;

        GLHDRTexture2D mSurfelsLuminanceMap;
        GLHDRTexture2D mSurfelClustersLuminanceMap;
        GLFramebuffer mSurfelsLuminanceFramebuffer;
        GLFramebuffer mSurfelClustersLuminanceFramebuffer;

        std::array<GLHDRTexture3D, 7> mGridProbesSHMaps;
        std::array<GLLDRTexture3D, 4> mGridProbesSHIntegerMaps;
        GLFramebuffer mGridProbesSHFramebuffer;

        GLDepthTexture2DArray mShadowMaps;
        GLDepthTextureCubemap mShadowCubeMap;
        GLFramebuffer mDepthFramebuffer;

        GLSLGridLightProbeRendering mGridProbeRenderingShader;
        GLSLFullScreenQuad mFSQuadShader;
        GLSLGenericGeometry mGenericShader;

        void setupGLState();
        void setupTextures();
        void setupFramebuffers();

        void bindDefaultFramebuffer();

        void performDepthPrepass();
        void renderShadowMapsForDirectionalLights();
        void relightSurfels();
        void averageSurfelClusterLuminances();
        void updateGridProbes();

        void convertEquirectangularMapToCubemap();
        void buildDiffuseIrradianceMap();
        void buildSpecularIrradianceMap();
        void buildBRDFIntegrationMap();

    public:
        SceneRenderer(const Scene* scene, std::shared_ptr<const SurfelData> surfelData, std::shared_ptr<const DiffuseLightProbeData> diffuseProbeData);
        
        void setDefaultRenderComponentsProvider(DefaultRenderComponentsProviding *provider);
        void setRenderingSettings(const RenderingSettings& settings);
        bool raySelectsMesh(const Ray3D& ray, ID& meshID);

        void prepareFrame();

        void renderMeshes();
        void renderSkybox();
        void renderSurfelsGBuffer();
        void renderSurfelLuminances();
        void renderSurfelClusterLuminances();
        void renderDiffuseGridProbes(float radius);
        void renderLinksForDiffuseProbe(size_t probeIndex);
        void renderProbeOcclusionMap(size_t probeIndex);
    };
    
}

#endif /* Renderer_hpp */
