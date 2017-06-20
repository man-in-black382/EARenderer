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
    
    const glm::vec2& Input::freeMousePosition() const {
        return mFreeMousePosition;
    }
    
    const glm::vec2& Input::draggedMousePosition() const {
        return mDraggedMousePosition;
    }
    
    const std::unordered_set<uint16_t>& Input::pressedKeyCodes() const {
        return mPressedKeyCodes;
    }
    
#pragma mark - Other methods
    
    void Input::updateMousePosition(const glm::vec2& position, MouseAction action) {
        // Prevent mouse jumping
        switch (action) {
            case MouseAction::pressDown:
            case MouseAction::pressUp:
            case MouseAction::drag: {
                mPreviousDraggedMousePosition = mDraggedMousePosition;
                mDraggedMousePosition = position;
                mShouldReturnDelta = true;
                break;
            }
                
            case MouseAction::freeMovement: {
                mFreeMousePosition = position;
                break;
            }
        }
    }
    
    void Input::registerKey(uint16_t code) {
        mPressedKeyCodes.insert(code);
    }
    
    void Input::unregisterKey(uint16_t code) {
        mPressedKeyCodes.erase(code);
    }
    
    glm::vec2 Input::draggedMouseDelta() {
        glm::vec2 delta = mShouldReturnDelta ? mDraggedMousePosition - mPreviousDraggedMousePosition : glm::zero<glm::vec2>();
        mShouldReturnDelta = false;
        return delta;
    }
    
    bool Input::isDirectionKeyPressed(Direction direction) const {
        uint16_t code = static_cast<std::underlying_type<Direction>::type>(direction);
        return mPressedKeyCodes.find(code) != mPressedKeyCodes.end();
    }
    
}
