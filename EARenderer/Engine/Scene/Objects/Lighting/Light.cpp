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

    bool Light::isEnabled() const {
        return mIsEnabled;
    }
    
#pragma mark - Setters
    
    void Light::setPosition(const glm::vec3& position) {
        mPosition = position;
    }
    
    void Light::setColor(const Color& color) {
        mColor = color;
    }

    void Light::setIsEnabled(bool enabled) {
        mIsEnabled = enabled;
    }
    
#pragma mark - Protected
    
    float Light::split(uint8_t cascadeIndex, uint8_t cascadesCount, float nearPlane, float farPlane, float lambda) const {
        // Formula https://habrahabr.ru/post/226421/
        float f = (float)cascadeIndex / (float)cascadesCount;
        float l = nearPlane * powf(farPlane / nearPlane, f);
        float u = nearPlane + (farPlane - nearPlane) * f;
        return l * lambda + u * (1.0f - lambda);
    }

}
