//
//  Settings.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 28.06.2018.
//  Copyright © 2018 MPO. All rights reserved.
//

#ifndef Settings_hpp
#define Settings_hpp

#include "SurfelRenderer.hpp"
#include "GaussianBlurSettings.hpp"
#include "BloomSettings.hpp"
#include "Size2D.hpp"
#include "Color.hpp"

namespace EARenderer {

    struct RenderingSettings {

        struct Mesh {
            bool materialsEnabled = true;
            bool globalIlluminationEnabled = true;
            bool lightMultibounceEnabled = true;
            bool meshRenderingEnabled = true;
            bool parallaxMappingEnabled = true;

            float parallaxMappingStrength = 0.003;

            uint32_t shadowCascadesCount = 1;
            GaussianBlurSettings shadowBlur{8, 8};

            uint32_t booleanBitmask() const {
                uint32_t bitmask = 0;
                bitmask |= materialsEnabled;
                bitmask <<= 1;
                bitmask |= globalIlluminationEnabled;
                bitmask <<= 1;
                bitmask |= lightMultibounceEnabled;
                bitmask <<= 1;
                bitmask |= meshRenderingEnabled;
                bitmask <<= 1;
                bitmask |= parallaxMappingEnabled;
                return bitmask;
            }
        };

        struct Surfel {
            bool renderingEnabled = false;
            SurfelRenderer::Mode renderingMode = SurfelRenderer::Mode::Default;
            int32_t POVProbeIndex = -1;
        };

        struct Probe {
            bool probeRenderingEnabled = false;
            int32_t clusterLinksRenderingProbeIndex = -1;
            float sphereRadius = 0.05;
        };

        Mesh meshSettings;
        Surfel surfelSettings;
        Probe probeSettings;
        BloomSettings bloomSettings;

        bool skyboxRenderingEnabled = true;
        bool triangleRenderingEnabled = false;

        Size2D displayedFrameResolution{1920, 1080};
        Size2D directionalShadowMapResolution{4096};
        Size2D omnidirectionalShadowMapResolution{2048};
        Size2D penumbraResolution{displayedFrameResolution.transformedBy(glm::vec2(1.0 / 2.0))};
    };

}

#endif /* Settings_hpp */
