//
//  FrustumCascadeBuilder.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 01.08.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "FrustumCascadeBuilder.hpp"
#include "AxisAlignedBox3D.hpp"

#include <glm/vec4.hpp>
#include <glm/gtx/extended_min_max.hpp>

#include <array>
#include <limits>

namespace EARenderer {
    
    FrustumCascades FrustumCascadeBuilder::subfrustumsForCameraInLightSpace(const Camera& camera, const DirectionalLight& light, int8_t numberOfCascades) {
        
        // Cascaded shadow maps http://ogldev.atspace.co.uk/www/tutorial49/tutorial49.html
        
        FrustumCascades cascades;
        
        float cameraPlanesDistance = camera.farClipPlane() - camera.nearClipPlane();
        float step = cameraPlanesDistance / numberOfCascades;
        float near = camera.nearClipPlane();
        float halfFOVH = camera.FOVH() / 2.f;
        float halfFOVV = camera.FOVV() / 2.f;
        
        glm::mat4 inverseCameraViewMat = glm::inverse(camera.viewMatrix());
        glm::mat4 lightViewMat = light.viewMatrix();
        
        for (int8_t i = 0; i < numberOfCascades; i++) {
            float far = near + step;
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
                point = lightViewMat * inverseCameraViewMat * point;
                box.min = glm::min(box.min, glm::vec3{point});
                box.max = glm::max(box.max, glm::vec3{point});
            }
            
            cascades.lightViewProjections.emplace_back(box.asFrustum());
            cascades.splits.emplace_back(far);
            
            near = far;
        }
        
        return cascades;
    }
    
}
