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
        GLFloatBufferTexture mLightProbeSHBufferTexture;
        GLIntegerBufferTexture mLightProbeIndicesBufferTexture;
        
        void captureEnvironmentForProbe(Scene *scene, const LightProbe& probe);
        
    public:
        GLHDRTextureCubemap mEnvironmentMap;
        
        LightProbeBuilder(const Size2D& probeCaptureResolution, uint32_t spaceDivisionResolution);
        
        void buildAndPlaceProbesInScene(Scene* scene);
    };
    
}

#endif /* LightProbeBuilder_hpp */
