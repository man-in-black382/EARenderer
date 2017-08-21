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
#include "GLSLDirectionalBlinnPhong.hpp"
#include "GLSLOmnidirectionalBlinnPhong.hpp"
#include "GLSLFullScreenQuad.hpp"
#include "GLDepthTexture2D.hpp"
#include "GLDepthTextureCubemap.hpp"
#include "GLDepthTexture2DArray.hpp"
#include "GLTexture2DArray.hpp"
#include "GLSLDirectionalDepth.hpp"
#include "GLSLOmnidirectionalDepth.hpp"
#include "GLSLSkybox.hpp"
#include "GLSLGenericGeometry.hpp"
#include "DefaultRenderComponentsProviding.hpp"
#include "FrustumCascades.hpp"
#include "Ray3D.hpp"

namespace EARenderer {
    
    class SceneRenderer {
    private:
        uint8_t mNumberOfCascades = 3;
        
        Scene *mScene = nullptr;
        DefaultRenderComponentsProviding *mDefaultRenderComponentsProvider = nullptr;
        
        GLSLDirectionalBlinnPhong mDirectionalBlinnPhongShader;
        GLSLOmnidirectionalBlinnPhong mOmnidirectionalBlinnPhongShader;
        GLSLDirectionalDepth mDirectionalDepthShader;
        GLSLOmnidirectionalDepth mOmnidirectionalDepthShader;
        GLSLSkybox mSkyboxShader;
        
        GLDepthTexture2DArray mCascadedShadowMaps;
        GLDepthTextureCubemap mShadowCubeMap;
        GLFramebuffer mDepthFramebuffer;
        
        // DEBUG
        GLSLFullScreenQuad mFSQuadShader;
        GLSLGenericGeometry mGenericShader;
        
        std::unordered_set<ID> mMeshesToHighlight;
        
        void renderShadowMapsForDirectionalLights(const FrustumCascades& cascades);
        void renderShadowMapsForPointLights();
        void renderDirectionalLighting(const FrustumCascades& cascades);
        void renderPointLighting();
        void renderSkybox();
        
    public:
        SceneRenderer(Scene* scene);
        
        void setDefaultRenderComponentsProvider(DefaultRenderComponentsProviding *provider);
        void setMeshHiglightEnabled(bool enabled, ID meshID);
        void disableMeshesHighlight();
        
        bool raySelectsMesh(const Ray3D& ray, ID& meshID);
        
        void render();
    };
    
}

#endif /* Renderer_hpp */
