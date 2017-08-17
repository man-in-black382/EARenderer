//
//  FrustumCascadeBuilder.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 01.08.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "FrustumCascadeBuilder.hpp"
#include "AxisAlignedBox3D.hpp"
#include "Log.hpp"

#include <glm/vec4.hpp>
#include <glm/gtx/extended_min_max.hpp>
#include <glm/gtx/transform.hpp>

#include <array>
#include <limits>

namespace EARenderer {
    
    float FrustumCascadeBuilder::split(uint8_t cascadeIndex, uint8_t cascadesCount, float nearPlane, float farPlane, float lambda) {
        
        // Formula https://habrahabr.ru/post/226421/
        
        float f = (float)cascadeIndex / (float)cascadesCount;
        float l = nearPlane * powf(farPlane / nearPlane, f);
        float u = nearPlane + (farPlane - nearPlane) * f;
        return l * lambda + u * (1.0f - lambda);
    }
    
    FrustumCascades FrustumCascadeBuilder::subfrustumsForCameraInLightSpace(const Camera& camera, const DirectionalLight& light, int8_t numberOfCascades) {
        
        // Cascaded shadow maps http://ogldev.atspace.co.uk/www/tutorial49/tutorial49.html
        
        FrustumCascades cascades;
        
        float halfFOVH = glm::radians(camera.FOVH() / 2.f);
        float halfFOVV = glm::radians(camera.FOVV() / 2.f);
        
        glm::mat4 inverseCameraViewMat = glm::inverse(camera.viewMatrix());
        glm::mat4 lightViewMat = light.viewMatrix();
        
        // NDC Z is in opposite direction, don't forget!
        float near = -camera.nearClipPlane();
        
        for (int8_t i = 0; i < numberOfCascades; i++) {
            // -, cuz we're going in the negative direction (Z)
            float far = -split(i + 1, numberOfCascades, camera.nearClipPlane(), camera.farClipPlane(), 0.9);
            float xn = near * tanf(halfFOVH);
            float xf = far * tanf(halfFOVH);
            float yn = near * tanf(halfFOVV);
            float yf = far * tanf(halfFOVV);
            
            // Construct corner points of subfrustum in camera view space
            std::array<glm::vec4, 8> cornerPoints {
                glm::vec4{ -xn, -yn, near, 1.f },
                glm::vec4{ -xn, yn, near, 1.f },
                glm::vec4{ xn, yn, near, 1.f },
                glm::vec4{ xn, -yn, near, 1.f },
                glm::vec4{ -xf, -yf, far, 1.f },
                glm::vec4{ -xf, yf, far, 1.f },
                glm::vec4{ xf, yf, far, 1.f },
                glm::vec4{ xf, -yf, far, 1.f }
            };
            
            AxisAlignedBox3D box{ glm::vec3{ std::numeric_limits<float>::max() }, glm::vec3{ std::numeric_limits<float>::lowest() } };
            
            for (auto& point : cornerPoints) {
                glm::vec4 p = point;
                p = glm::inverse(lightViewMat) * inverseCameraViewMat * p;
                box.min = glm::min(box.min, glm::vec3{p});
                box.max = glm::max(box.max, glm::vec3{p});
            }

            cascades.lightViewProjections.emplace_back(box.asFrustum() * lightViewMat);
            
            // DEBUG
            for (auto& point : cornerPoints) {
                point = inverseCameraViewMat * point;
            }
            cascades.cornerPoints.emplace_back(cornerPoints);
            cascades.boxes.emplace_back(box);
            Log::logConsole(box);
            
            
            // Depth is not linear when perspective projection is applied
            // Project splits first so shader code could understand it
            glm::vec4 dv = camera.projectionMatrix() * glm::vec4{ 0.0, 0.0, far, 1.0 };
            dv /= dv.w;
            cascades.splits.emplace_back(dv.z);
            
            near = far;
        }
        
        printf("\n\n");
        
        return cascades;
    }
    
}
