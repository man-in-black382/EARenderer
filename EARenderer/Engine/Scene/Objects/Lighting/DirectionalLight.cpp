//
//  DirectionalLight.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 30.04.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "DirectionalLight.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_access.hpp>

#include <array>

namespace EARenderer {
    
#pragma mark - Lifecycle
    
    DirectionalLight::DirectionalLight(const Color& color,
                                       const glm::vec3& direction)
    :
    Light(glm::zero<glm::vec3>(), color),
    mDirection(direction)
    { }
    
    DirectionalLight::DirectionalLight(const Color& color,
                                       const glm::vec3& direction,
                                       float frustumSize)
    :
    Light(glm::zero<glm::vec3>(), color),
    mDirection(direction),
    mFrustumSize(frustumSize)
    { }
    
#pragma mark - Getters
    
    const glm::vec3& DirectionalLight::direction() const {
        return mDirection;
    }
    
    void DirectionalLight::setDirection(const glm::vec3& direction) {
        mDirection = direction;
    }
    
#pragma mark - Math
    
    glm::mat4 DirectionalLight::viewMatrix() const {
        // Avoid gimbal locks
        glm::vec3 up{ 0.0, 1.0, 0.0 };
        glm::vec3 front{ 0.0, 0.0, 1.0 };
        float dot = glm::dot(mDirection, up);
        glm::vec3 reference = (1.f - fabs(dot)) < 0.01f ? front : up;
        
        return glm::lookAt(glm::zero<glm::vec3>(), mDirection, reference);
    }
    
    FrustumCascades DirectionalLight::cascadesForCamera(const Camera& camera, uint8_t numberOfCascades) const {
        // Cascaded shadow maps http://ogldev.atspace.co.uk/www/tutorial49/tutorial49.html
        
        FrustumCascades cascades;
        
        float tanFOVV = tanf(glm::radians(camera.FOVV() / 2.f));
        float tanFOVH = tanf(glm::radians(camera.FOVH() / 2.f));
        
        glm::mat4 inverseCameraViewMat = glm::inverse(camera.viewMatrix());
        glm::mat4 lightViewMat = viewMatrix();
        
        // NDC Z is in opposite direction, don't forget!
        float near = -camera.nearClipPlane();
        
        for (int8_t i = 0; i < numberOfCascades; i++) {
            // -, cuz we're going in the negative direction (Z)
            float far = -split(i + 1, numberOfCascades, camera.nearClipPlane(), camera.farClipPlane(), 0.9);
            
            float xn = near * tanFOVH;
            float xf = far * tanFOVH;
            float yn = near * tanFOVV;
            float yf = far * tanFOVV;
            
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
                p = lightViewMat * inverseCameraViewMat * p;
                box.min = glm::min(box.min, glm::vec3{p});
                box.max = glm::max(box.max, glm::vec3{p});
            }
            
            cascades.lightViewProjections.emplace_back(box.asFrustum() * lightViewMat);
            
            // Depth is not linear when perspective projection is applied
            // Project splits first so shader code could understand it
            glm::vec4 dv = camera.projectionMatrix() * glm::vec4{ 0.0, 0.0, far, 1.0 };
            dv /= dv.w;
            cascades.splits.emplace_back(dv.z);
            
            near = far;
        }
        
        return cascades;
    }
    
}
