//
//  LightProbe.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 18.11.2017.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "LightProbe.hpp"

#include <math.h>

namespace EARenderer {
    
#pragma mark - Lifecycle
    
    LightProbe::LightProbe(const glm::vec3& position)
    :
    position(position)
    { }
    
#pragma mark - Private heplers
    
    float LightProbe::solidAngle(int32_t x, int32_t y, const Size2D& cubeSize) const {
        float s = ((float)x + 0.5f) / cubeSize.width * 2.0f - 1.0f;
        float t = ((float)y + 0.5f) / cubeSize.height * 2.0f - 1.0f;
        
        float halfTexelSizeX = 1.0f / cubeSize.width;
        float halfTexelSizeY = 1.0f / cubeSize.height;
        
        float x0 = s - halfTexelSizeX;
        float y0 = t - halfTexelSizeY;
        float x1 = s + halfTexelSizeX;
        float y1 = t + halfTexelSizeY;
        
        return areaIntegral(x0, y0) - areaIntegral(x0, y1) - areaIntegral(x1, y0) + areaIntegral(x1, y1);
    }
    
    float LightProbe::areaIntegral(float x, float y) const {
        return atan2f(x*y, sqrtf(x*x + y*y + 1));
    }
    
#pragma mark - Getters
    
    const SphericalHarmonics& LightProbe::sphericalHarmonics() const {
        return mSphericalHarmonics;
    }
    
#pragma mark -

    void LightProbe::updateSHCoefficients(const GLHDRTextureCubemap& cubemap) {
        mSphericalHarmonics = SphericalHarmonics();

        for (int8_t face = 0; face < 6; face++) {
            for (int32_t y = 0; y < cubemap.size().height; y++) {
                for (int32_t x = 0; x < cubemap.size().width; x++) {
                    cubemap.sampleTexels([&](const GLCubemapSampler& sampler) {
                        GLCubemapFace cubeFace = static_cast<GLCubemapFace>(face);
                        Color texel = sampler.sample(cubeFace, x, y);
                        float sAngle = solidAngle(x, y, cubemap.size());
                        glm::vec3 direction;
                        sampler.computeSampleVector(cubeFace, x, y, cubemap.size(), direction);
                        mSphericalHarmonics.contribute(direction, texel, sAngle);
                    });
                }
            }
        }
    }
}
