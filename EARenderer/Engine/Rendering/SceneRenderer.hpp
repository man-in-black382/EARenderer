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

#include "Scene.hpp"
#include "GLFramebuffer.hpp"
#include "DefaultRenderComponentsProviding.hpp"
#include "FrustumCascades.hpp"
#include "Ray3D.hpp"

#include "GLSLCookTorrance.hpp"
#include "GLSLDirectionalBlinnPhong.hpp"
#include "GLSLOmnidirectionalBlinnPhong.hpp"
#include "GLSLFullScreenQuad.hpp"
#include "GLSLDirectionalDepth.hpp"
#include "GLSLOmnidirectionalDepth.hpp"
#include "GLSLSkybox.hpp"
#include "GLSLGenericGeometry.hpp"
#include "GLSLEquirectangularMapConversion.hpp"

#include "GLDepthTexture2D.hpp"
#include "GLDepthTextureCubemap.hpp"
#include "GLDepthTexture2DArray.hpp"
#include "GLHDRTexture2D.hpp"
#include "GLHDRTextureCubemap.hpp"
#include "GLTexture2DArray.hpp"

namespace EARenderer {
    
    class SceneRenderer {
    private:
        uint8_t mNumberOfCascades = 3;
        
        Scene *mScene = nullptr;
        DefaultRenderComponentsProviding *mDefaultRenderComponentsProvider = nullptr;
        
        GLSLCookTorrance mCookTorranceShader;
        GLSLDirectionalBlinnPhong mDirectionalBlinnPhongShader;
        GLSLOmnidirectionalBlinnPhong mOmnidirectionalBlinnPhongShader;
        GLSLDirectionalDepth mDirectionalDepthShader;
        GLSLOmnidirectionalDepth mOmnidirectionalDepthShader;
        GLSLSkybox mSkyboxShader;
        GLSLEquirectangularMapConversion mEqurectangularMapConversionShader;
        
        GLHDRTexture2D mHDREqurectangularSkybox;
        GLTextureCubemap mHDRSkybox;
        GLHDRTextureCubemap mHRDIrradianceMap;
        GLFramebuffer mIBLFramebuffer;
        
        GLDepthTexture2DArray mCascadedShadowMaps;
        GLDepthTextureCubemap mShadowCubeMap;
        GLFramebuffer mDepthFramebuffer;
        
        // DEBUG
        GLSLFullScreenQuad mFSQuadShader;
        GLSLGenericGeometry mGenericShader;
        //
        
        void renderShadowMapsForDirectionalLights(const FrustumCascades& cascades);
        void renderShadowMapsForPointLights();
        void renderDirectionalLighting(const FrustumCascades& cascades);
        void renderPointLighting();
        void renderSkybox();
        
        void renderClassicMeshes();
        
        void convertEquirectangularMapToCubemap();
        
        void renderPBRMeshes();
        
    public:
        SceneRenderer(Scene* scene, const std::string& equirectangularSkyboxPath);
        
        void setDefaultRenderComponentsProvider(DefaultRenderComponentsProviding *provider);
        bool raySelectsMesh(const Ray3D& ray, ID& meshID);
        void render();
    };
    
}

#endif /* Renderer_hpp */
