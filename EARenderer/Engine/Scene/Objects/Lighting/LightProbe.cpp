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
    
#pragma mark - Public interface
    
    void LightProbe::updateSHCoefficients(const GLHDRTextureCubemap& cubemap) {
        SHCoefficients = SphericalHarmonics();
        
        // An Efficient Representation for Irradiance Environment Maps using spherical harmonics
        // by Ravi Ramamoorthi & Pat Hanrahan
        // https://graphics.stanford.edu/papers/envmap/envmap.pdf
        
        const float C0 = 0.28209479177387814347f; // 1 / (2*sqrt(pi))
        const float C1 = 0.48860251190291992159f; // sqrt(3 /(4pi))
        const float C2 = 1.09254843059207907054f; // 1/2 * sqrt(15/pi)
        const float C3 = 0.31539156525252000603f; // 1/4 * sqrt(5/pi)
        const float C4 = 0.54627421529603953527f; // 1/4 * sqrt(15/pi)
        
        for (int8_t face = 0; face < 6; face++) {
            for (int32_t y = 0; y < cubemap.size().height; y++) {
                for (int32_t x = 0; x < cubemap.size().width; x++) {
                    cubemap.sampleTexels([&](const GLCubemapSampler& sampler) {
                        GLCubemapFace face = static_cast<GLCubemapFace>(face);
                        glm::vec3 texel = sampler.sample(face, x, y).rgb();
                        float solAngle = solidAngle(x, y, cubemap.size());
                        glm::vec3 direction;
                        sampler.computeSampleVector(face, x, y, cubemap.size(), direction);
                        
                        // l, m = 0, 0
                        SHCoefficients[0] += texel * C0 * solAngle;
                        
                        // l, m = 1, -1
                        SHCoefficients[1] += texel * C1 * direction.y * solAngle;
                        // l, m = 1, 0
                        SHCoefficients[2] += texel * C1 * direction.z * solAngle;
                        // l, m = 1, 1
                        SHCoefficients[3] += texel * C1 * direction.x * solAngle;
                        
                        // l, m = 2, -2
                        SHCoefficients[4] += texel * C2 * (direction.x * direction.y) * solAngle;
                        // l, m = 2, -1
                        SHCoefficients[5] += texel * C2 * (direction.y * direction.z) * solAngle;
                        // l, m = 2, 0
                        SHCoefficients[6] += texel * C3 * (3.0f * direction.z * direction.z - 1.0f) * solAngle;
                        // l, m = 2, 1
                        SHCoefficients[7] += texel * C2 * (direction.x * direction.z) * solAngle;
                        // l, m = 2, 2
                        SHCoefficients[8] += texel * C4 * (direction.x * direction.x - direction.y * direction.y) * solAngle;
                    });

                }
            }
        }
    }
}
