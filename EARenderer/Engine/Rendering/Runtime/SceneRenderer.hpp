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

#include <glm/vec2.hpp>

#include "Scene.hpp"
#include "GLFramebuffer.hpp"
#include "DefaultRenderComponentsProviding.hpp"
#include "FrustumCascades.hpp"
#include "Ray3D.hpp"
#include "LightProbeBuilder.hpp"

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
#include "GLSLLightProbeRendering.hpp"
#include "GLSLLightProbeLinksRendering.hpp"

#include "GLDepthTexture2D.hpp"
#include "GLDepthTextureCubemap.hpp"
#include "GLHDRTexture2DArray.hpp"
#include "GLDepthTexture2DArray.hpp"
#include "GLHDRTexture2D.hpp"
#include "GLHDRTextureCubemap.hpp"
#include "GLTexture2DArray.hpp"
#include "GLHDRTexture3D.hpp"
#include "GLBufferTexture.hpp"

namespace EARenderer {
    
    class SceneRenderer {
    private:
        uint8_t mNumberOfCascades = 1;
        uint8_t mNumberOfIrradianceMips = 5;
        glm::ivec3 mProbeGridResolution;
        
        Scene *mScene = nullptr;
        DefaultRenderComponentsProviding *mDefaultRenderComponentsProvider = nullptr;
        GLVertexArray<DiffuseLightProbe> mDiffuseProbesVAO;
    
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
        
        GLHDRTextureCubemap mEnvironmentMapCube;
        GLHDRTextureCubemap mDiffuseIrradianceMap;
        GLHDRTextureCubemap mSpecularIrradianceMap;
        GLHDRTexture2D mBRDFIntegrationMap;
        GLFramebuffer mIBLFramebuffer;

        GLHDRTexture2DArray mSurfelsGBuffer;
        GLLDRTexture2D mSurfelClustersGBuffer;
        GLHDRTexture2D mSurfelsLuminanceMap;
        GLHDRTexture2D mSurfelClustersLuminanceMap;
        GLFramebuffer mSurfelsLuminanceFramebuffer;
        GLFramebuffer mSurfelClustersLuminanceFramebuffer;

        GLFloat3BufferTexture<glm::vec3> mSurfelClusterCentersBufferTexture;
        GLFloat3BufferTexture<SphericalHarmonics> mProjectionClusterSHsBufferTexture;
        GLUIntegerBufferTexture<uint32_t> mProjectionClusterIndicesBufferTexture;
        GLUIntegerBufferTexture<uint32_t> mDiffuseProbeClusterProjectionsBufferTexture;
        std::array<GLHDRTexture3D, 7> mGridProbesSphericalHarmonicMaps;
        GLFramebuffer mGridProbesFramebuffer;

        GLDepthTexture2DArray mShadowMaps;
        GLDepthTextureCubemap mShadowCubeMap;
        GLFramebuffer mDepthFramebuffer;

        GLSLLightProbeRendering mDiffuseProbeRenderingShader;
        GLSLFullScreenQuad mFSQuadShader;
        GLSLGenericGeometry mGenericShader;

        void setupGLState();
        void setupTextures();
        void setupFramebuffers();

        std::vector<std::vector<glm::vec3>> surfelsGBufferData() const;
        std::vector<uint8_t> surfelClustersGBufferData() const;
        std::vector<glm::vec3> surfelClusterCenters() const;
        std::vector<SphericalHarmonics> surfelProjectionsSH() const;
        std::vector<uint32_t> surfelClusterIndices() const;
        std::vector<uint32_t> probeProjectionsMetadata() const;

        void renderShadowMapsForDirectionalLights(const FrustumCascades& cascades);
        void relightSurfels(const FrustumCascades& cascades);
        void averageSurfelClusterLuminances();
        void updateGridProbes();
        void renderSkybox();
        
        void convertEquirectangularMapToCubemap();
        void buildDiffuseIrradianceMap();
        void buildSpecularIrradianceMap();
        void buildBRDFIntegrationMap();

    public:
        SceneRenderer(Scene* scene);
        
        void setDefaultRenderComponentsProvider(DefaultRenderComponentsProviding *provider);
        bool raySelectsMesh(const Ray3D& ray, ID& meshID);
        void renderMeshes();

        void renderSurfelsGBuffer();
        void renderSurfelLuminances();
        void renderSurfelClusterLuminances();
        void renderDiffuseProbes(float radius);
        void renderLinksForDiffuseProbe(size_t probeIndex);
    };
    
}

#endif /* Renderer_hpp */
