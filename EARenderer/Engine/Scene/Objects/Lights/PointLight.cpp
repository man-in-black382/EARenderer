//
//  PointLight.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 25.07.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "PointLight.hpp"

#include <glm/gtc/matrix_transform.hpp>

namespace EARenderer {
    
#pragma mark - Lifecycle
    
    PointLight::PointLight(const glm::vec3& position, const Color& color, float clipDistance)
    :
    Light(position, color),
    mClipDistance(clipDistance)
    { }
    
    std::array<glm::mat4, 6> PointLight::viewProjectionMatrices() const {
        glm::mat4 projMat = glm::perspective(glm::radians(90.f), 1.f, 0.01f, mClipDistance);
        return {
            projMat * glm::lookAt(mPosition, mPosition + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)),
            projMat * glm::lookAt(mPosition, mPosition + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)),
            projMat * glm::lookAt(mPosition, mPosition + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)),
            projMat * glm::lookAt(mPosition, mPosition + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0)),
            projMat * glm::lookAt(mPosition, mPosition + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0)),
            projMat * glm::lookAt(mPosition, mPosition + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0)),
        };
    }
    
    float PointLight::clipDistance() const {
        return mClipDistance;
    }
    
}
