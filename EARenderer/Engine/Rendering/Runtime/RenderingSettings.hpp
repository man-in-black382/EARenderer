//
//  RenderingSettings.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 30.05.2018.
//  Copyright © 2018 MPO. All rights reserved.
//

#ifndef RenderingSettings_hpp
#define RenderingSettings_hpp

#include "SurfelRenderer.hpp"

namespace EARenderer {

//    self.sceneRenderer->prepareFrame();
    //        glFinish();
    //    });

    //    EARenderer::Measurement::ExecutionTime("Mesh rendering took", [&]() {
//    self.sceneRenderer->renderMeshes();
    //        glFinish();
    //    });

//    self.sceneRenderer->renderDiffuseGridProbes(0.05);
    //    self.sceneRenderer->renderProbeOcclusionMap(500);

    //    self.sceneRenderer->renderSkybox();


    struct RenderingSettings {
        struct SurfelSettings {
            bool renderingEnabled = false;
            SurfelRenderer::Mode renderingMode = SurfelRenderer::Mode::Default;
            size_t POVProbeIndex = -1;
        };

        struct ProbeSettings {
            bool lightMultibounceEnabled = false;
            bool probeRenderingEnabled = false;
            size_t clusterLinksRenderingProbeIndex = -1;
            float sphereRadius = 0.05;
        };

        bool meshRenderingEnabled = true;
        bool skyboxRenderingEnabled = true;
        bool triangleRenderingEnabled = false;
    };

}

#endif /* RenderingSettings_hpp */
