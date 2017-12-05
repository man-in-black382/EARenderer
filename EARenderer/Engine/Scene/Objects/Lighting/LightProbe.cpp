//
//  LightProbe.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 18.11.2017.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "LightProbe.hpp"

#include <math.h>
#include <glm/gtc/matrix_transform.hpp>

namespace EARenderer {
    
#pragma mark - Lifecycle
    
    LightProbe::LightProbe(const glm::vec3& position, float farClipPlane)
    :
    position(position),
    mFarClipPlane(farClipPlane)
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

    const std::array<glm::mat4, 6> LightProbe::viewProjectionMatrices() const {
        glm::mat4 projMat = glm::perspective(glm::radians(90.f), 1.0f, 0.01f, mFarClipPlane);
        
        glm::vec3 XNegative = { -1.0, 0.0, 0.0 };
        glm::vec3 XPositive = { 1.0, 0.0, 0.0 };
        glm::vec3 YNegative = { 0.0, -1.0, 0.0 };
        glm::vec3 YPositive = { 0.0, 1.0, 0.0 };
        glm::vec3 ZNegative = { 0.0, 0.0, -1.0 };
        glm::vec3 ZPositive = { 0.0, 0.0, 1.0 };
        
        return {
            projMat * glm::lookAt(position, XNegative, glm::vec3(0.0, -1.0, 0.0)),
            projMat * glm::lookAt(position, XPositive, glm::vec3(0.0, -1.0, 0.0)),
            projMat * glm::lookAt(position, YPositive, glm::vec3(0.0, 0.0, 1.0)),
            projMat * glm::lookAt(position, YNegative, glm::vec3(0.0, 0.0, -1.0)),
            projMat * glm::lookAt(position, ZPositive, glm::vec3(0.0, -1.0, 0.0)),
            projMat * glm::lookAt(position, ZNegative, glm::vec3(0.0, -1.0, 0.0))
        };
    }
    
    const SphericalHarmonics& LightProbe::sphericalHarmonics() const {
        return mSphericalHarmonics;
    }
    
#pragma mark -

    void LightProbe::updateSHCoefficients(const GLHDRTextureCubemap& cubemap) {
        mSphericalHarmonics = SphericalHarmonics();

        cubemap.sampleTexels([&](const GLCubemapSampler& sampler) {
            for (int8_t face = 0; face < 6; face++) {
                for (int32_t y = 0; y < cubemap.size().height; y++) {
                    for (int32_t x = 0; x < cubemap.size().width; x++) {
                        Color texel = sampler.sample(CubemapFaceFromIndex(face), x, y);
                        printf("Sampled texel: r %f g %f b %f \n", texel.r, texel.g, texel.b);
                        float sAngle = solidAngle(x, y, cubemap.size());
                        glm::vec3 direction;
                        sampler.computeSampleVector(CubemapFaceFromIndex(face), x, y, cubemap.size(), direction);
                        mSphericalHarmonics.contribute(direction, texel, sAngle);
                    }
                }
            }
        });
    }
}
