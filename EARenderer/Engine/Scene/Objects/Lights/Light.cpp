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

#pragma mark - Lifecycle
    
    Light::Light()
    :
    mPosition(glm::zero<glm::vec3>()),
    mColor(Color::white())
    { }
    
    Light::Light(const glm::vec3& position, const Color& color)
    :
    mPosition(position),
    mColor(color)
    { }
    
#pragma mark - Getters
    
    const glm::vec3& Light::position() const {
        return mPosition;
    }
    
    const Color& Light::color() const {
        return mColor;
    }
    
#pragma mark - Setters
    
    void Light::setPosition(const glm::vec3& position) {
        mPosition = position;
    }
    
    void Light::setColor(const Color& color) {
        mColor = color;
    }
    
}
