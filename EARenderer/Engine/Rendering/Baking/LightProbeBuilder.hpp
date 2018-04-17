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
#include "Vertex1P1N2UV1T1BT.hpp"

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

namespace EARenderer {
    
    class LightProbeBuilder {
    private:
        GLFramebuffer mFramebuffer;
        GLDepthRenderbuffer mDepthRenderbuffer;
        GLDepthTextureCubemap mDepthCubemap;
        GLSLLightProbeEnvironmentCapture mEnvironmentCaptureShader;
        uint32_t mSpaceDivisionResolution;
        
        void captureEnvironmentForProbe(Scene *scene, const LightProbe& probe);

        float surfelSolidAngle(Scene *scene, const Surfel& surfel, const DiffuseLightProbe& probe);

        SurfelClusterProjection projectSurfelCluster(Scene *scene, const SurfelCluster& cluster, const DiffuseLightProbe& probe);

        void projectSurfelClustersOnProbe(Scene* scene, DiffuseLightProbe& probe);

        void generateProbesForStaticVertices(Scene *scene,
                                             const glm::vec2& lightmapResolution,
                                             const glm::mat4& modelMatrix,
                                             const Vertex1P1N2UV1T1BT& vertex0,
                                             const Vertex1P1N2UV1T1BT& vertex1,
                                             const Vertex1P1N2UV1T1BT& vertex2);
        
    public:
        GLHDRTextureCubemap mEnvironmentMap;
        
        LightProbeBuilder(const Size2D& probeCaptureResolution);
        
        void buildAndPlaceProbesInScene(Scene *scene);
        void buildDynamicGeometryProbes(Scene *scene);
        void buildStaticGeometryProbes(Scene *scene);
    };
    
}

#endif /* LightProbeBuilder_hpp */
