//
//  Input.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 11.06.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "Input.hpp"

#include <glm/gtc/constants.hpp>

namespace EARenderer {
    
#pragma mark - Lifecycle
    
    Input::Input() { }
    Input::~Input() { }
    
    Input& Input::shared() {
        static Input i;
        return i;
    }
    
#pragma mark - Getters
    
    glm::vec2 Input::mouseDelta() {
        glm::vec2 delta = mShouldReturnDelta ? mMousePosition - mPreviousMousePosition : glm::zero<glm::vec2>();
        mShouldReturnDelta = false;
        return delta;
    }
    
    const glm::vec2& Input::mousePosition() const {
        return mMousePosition;
    }
    
    const std::unordered_set<uint16_t>& Input::pressedKeyCodes() const {
        return mPressedKeyCodes;
    }
    
#pragma mark - Other methods
    
    void Input::updateMousePosition(const glm::vec2& position, bool dropPreviousPosition) {
        // Prevent mouse jumping
        mPreviousMousePosition = dropPreviousPosition ? position : mMousePosition;
        mMousePosition = position;
        mShouldReturnDelta = true;
    }
    
    void Input::registerKey(uint16_t code) {
        mPressedKeyCodes.insert(code);
    }
    
    void Input::unregisterKey(uint16_t code) {
        mPressedKeyCodes.erase(code);
    }
    
    bool Input::isDirectionKeyPressed(InputDirection direction) {
        uint16_t code = static_cast<std::underlying_type<InputDirection>::type>(direction);
        return mPressedKeyCodes.find(code) != mPressedKeyCodes.end();
    }
    
}
