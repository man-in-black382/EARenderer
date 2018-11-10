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

            float parallaxMappingStrength = 0.05;

            uint32_t shadowCascadesCount = 4;
            float ESMFactor = 80.0;
            GaussianBlurSettings shadowBlur { 8, 8 };
            
            Color skyColor = Color(0.0, 0.0623, 0.1);

            uint32_t booleanBitmask() const {
                uint32_t bitmask = 0;
                bitmask |= materialsEnabled; bitmask <<= 1;
                bitmask |= globalIlluminationEnabled; bitmask <<= 1;
                bitmask |= lightMultibounceEnabled; bitmask <<= 1;
                bitmask |= meshRenderingEnabled; bitmask <<= 1;
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

        Size2D resolution { 1920, 1080 };
    };

}

#endif /* Settings_hpp */
