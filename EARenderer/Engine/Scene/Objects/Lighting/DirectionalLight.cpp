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
#include <glm/gtx/transform.hpp>

#include <array>
#include "Sphere.hpp"

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
    
    FrustumCascades DirectionalLight::cascadesForCamera(const Camera& camera, uint8_t numberOfCascades, const glm::vec3& scale) const {
        // Cascaded shadow maps http://ogldev.atspace.co.uk/www/tutorial49/tutorial49.html
        
        FrustumCascades cascades;
        cascades.amount = numberOfCascades;
        
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

            glm::mat4 scaleMat = glm::scale(scale);

            for (auto& point : cornerPoints) {
                glm::vec4 p = point;
                p = lightViewMat * scaleMat * inverseCameraViewMat * p;
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

        cascades.splitSpaceMatrix = camera.viewProjectionMatrix();
        
        return cascades;
    }
    
    FrustumCascades DirectionalLight::cascadesForWorldBoundingBox(const AxisAlignedBox3D& box, uint8_t numberOfCascades) const {
        FrustumCascades cascades;
        cascades.amount = numberOfCascades;

        glm::mat4 lightViewMat = viewMatrix();
        AxisAlignedBox3D frustum{ glm::vec3{ std::numeric_limits<float>::max() }, glm::vec3{ std::numeric_limits<float>::lowest() } };

        // Calculate rotationally invariant box to solve shimmering issues
        glm::vec3 frustumCenter = box.center();
        glm::vec3 diff = frustumCenter - box.min;

        float describedSphereRadius = glm::length(diff);

        glm::vec4 lightSpaceFrustumCenter = lightViewMat * glm::vec4(frustumCenter, 1.0);
        frustum.min = lightSpaceFrustumCenter - describedSphereRadius;
        frustum.max = lightSpaceFrustumCenter + describedSphereRadius;

        float zDelta = frustum.max.z - frustum.min.z;
        float zStep = zDelta / numberOfCascades;

        // Split frustum
        for (int8_t i = 0; i < numberOfCascades; i++) {
            glm::vec3 min(frustum.min.x, frustum.min.y, frustum.min.z + zStep * i);
            glm::vec3 max(frustum.max.x, frustum.max.y, frustum.min.z + zStep * (i + 1));
            AxisAlignedBox3D cascade(min, max);

            cascades.lightSpaceCascades.emplace_back(cascade);
            cascades.lightViewProjections.emplace_back(cascade.asFrustum() * lightViewMat);

            float split = zStep / zDelta * (i + 1);
            // Transform to [-1; 1] range
            split = split * 2.0 - 1.0;
            // Account for OpenGL handedness by inverting split distance, which is located on the Z axis
//            split *= -1;

            cascades.splits.push_back(split);
        }

        cascades.splitSpaceMatrix = frustum.asFrustum() * lightViewMat;
        
        return cascades;
    }
    
}
