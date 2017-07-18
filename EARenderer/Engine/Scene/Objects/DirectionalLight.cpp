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
    
    DirectionalLight::DirectionalLight(const glm::vec3& position,
                                       const glm::vec3& color,
                                       const glm::vec3& direction)
    :
    Light(position, color),
    mDirection(direction)
    { }
    
    glm::mat4 DirectionalLight::viewProjectionMatrix() const {
        glm::mat4 view = glm::lookAt(mPosition, mDirection, { 0.0, 1.0, 0.0 });
        glm::mat4 projection = glm::ortho(-10.0, 10.0, -10.0, 10.0, 1.0, 7.5);
        return projection * view;
    }
    
    const glm::vec3& DirectionalLight::direction() const {
        return mDirection;
    }
    
    void DirectionalLight::setDirection(const glm::vec3& direction) {
        mDirection = direction;
    }
    
}
