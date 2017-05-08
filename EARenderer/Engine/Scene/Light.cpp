//
//  Light.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 25.03.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "Light.hpp"

#include <glm/gtc/constants.hpp>

namespace EARenderer {

    Light::Light()
    :
    mPosition(glm::zero<glm::vec3>()),
    mColor(glm::one<glm::vec4>())
    { }
    
    Light::Light(const glm::vec3& position, const glm::vec3& color)
    :
    mPosition(position),
    mColor(color)
    { }
    
    
    
    const glm::vec3& Light::position() const {
        return mPosition;
    }
    
    const glm::vec3& Light::color() const {
        return mColor;
    }
    
    void Light::setPosition(const glm::vec3& position) {
        mPosition = position;
    }
    
    void Light::setColor(const glm::vec3 &color) {
        mColor = color;
    }
    
}
