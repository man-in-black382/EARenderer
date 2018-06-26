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

    struct RenderingSettings {

        struct MeshSettings {

            enum class SphericalHarmonicsCompression { Uncompressed, Compressed322, Compressed311 };

            bool materialsEnabled = true;
            bool globalIlluminationEnabled = true;
            bool lightMultibounceEnabled = true;
            bool meshRenderingEnabled = true;
            bool parallaxMappingEnabled = true;

            SphericalHarmonicsCompression SHCompression = SphericalHarmonicsCompression::Compressed311;

            float parallaxMappingStrength = 0.05;

            uint32_t shadowCascadesCount = 4;
            float ESMFactor = 0.5;
            float ESMDarkeningFactor = 1.0;
            uint32_t shadowBlurRadius = 8;

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

        struct SurfelSettings {
            bool renderingEnabled = false;
            SurfelRenderer::Mode renderingMode = SurfelRenderer::Mode::Default;
            int32_t POVProbeIndex = -1;
        };

        struct ProbeSettings {
            bool probeRenderingEnabled = false;
            int32_t clusterLinksRenderingProbeIndex = -1;
            float sphereRadius = 0.05;
        };

        MeshSettings meshSettings;
        SurfelSettings surfelSettings;
        ProbeSettings probeSettings;

        bool skyboxRenderingEnabled = true;
        bool triangleRenderingEnabled = false;
    };

}

#endif /* RenderingSettings_hpp */
