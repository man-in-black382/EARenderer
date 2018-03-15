//
//  LightProbeBuilder.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 18.11.2017.
//  Copyright © 2017 MPO. All rights reserved.
//

#ifndef LightProbeBuilder_hpp
#define LightProbeBuilder_hpp

#include "Scene.hpp"
#include "GLHDRTextureCubemap.hpp"
#include "GLFramebuffer.hpp"
#include "GLBufferTexture.hpp"
#include "GLDepthRenderbuffer.hpp"
#include "GLSLLightProbeEnvironmentCapture.hpp"

namespace EARenderer {
    
    class LightProbeBuilder {
    private:
        GLFramebuffer mFramebuffer;
        GLDepthRenderbuffer mDepthRenderbuffer;
        GLDepthTextureCubemap mDepthCubemap;
        GLSLLightProbeEnvironmentCapture mEnvironmentCaptureShader;
        uint32_t mSpaceDivisionResolution;
        
        void captureEnvironmentForProbe(Scene *scene, const LightProbe& probe);
        float surfelSolidAngle(Scene *scene, const Surfel& surfel, const glm::vec3& standpoint);
        SurfelClusterProjection projectSurfelCluster(Scene *scene, const SurfelCluster& cluster, const glm::vec3& standpoint);
        
    public:
        GLHDRTextureCubemap mEnvironmentMap;
        
        LightProbeBuilder(const Size2D& probeCaptureResolution, uint32_t spaceDivisionResolution);
        
        void buildAndPlaceProbesInScene(Scene* scene);
        void buildAndPlaceProbesForDynamicGeometry(Scene *scene);
    };
    
}

#endif /* LightProbeBuilder_hpp */
