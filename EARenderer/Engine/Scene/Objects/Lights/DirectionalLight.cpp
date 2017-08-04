//
//  DirectionalLight.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 30.04.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "DirectionalLight.hpp"

#include <glm/gtc/matrix_transform.hpp>

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
        return glm::lookAt(mPosition, mDirection, { 0.0, 1.0, 0.0 });
    }
    
    glm::mat4 DirectionalLight::projectionMatrix() const {
        float halfFrustum = mFrustumSize / 2.f;
        return glm::ortho(-halfFrustum, halfFrustum, -halfFrustum, halfFrustum, 0.01f, halfFrustum);
    }
    
    glm::mat4 DirectionalLight::viewProjectionMatrix() const {
        return projectionMatrix() * viewMatrix();
    }
    
}
