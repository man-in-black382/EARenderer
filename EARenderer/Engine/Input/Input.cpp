//
//  Input.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 11.06.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "Input.hpp"

#include <type_traits>

#include <glm/gtc/constants.hpp>

namespace EARenderer {
    
#pragma mark - Lifecycle
    
    Input& Input::shared() {
        static Input i;
        return i;
    }
    
#pragma mark - Getters
    
    Input::MouseEvent& Input::mouseEvent() {
        return mMouseEvent;
    }
    
    Input::KeyboardEvent& Input::keyboardEvent() {
        return mKeyboardEvent;
    }
    
    const glm::vec2& Input::mousePosition() const {
        return mMousePosition;
    }
    
    const Input::KeyCode Input::pressedMouseButtonsMask() const {
        return mPressedMouseButtonsMask;
    }
    
    const Input::KeySet& Input::pressedKeyboardButtons() const {
        return mPressedKeyboardKeys;
    }
    
#pragma mark - Other methods
    
    void Input::registerMouseAction(MouseAction action, const glm::vec2& position, KeyCode keysMask) {
        mMousePosition = position;
        mPressedMouseButtonsMask = keysMask;
        mMouseEvent[action](this);
    }

    void Input::registerKey(KeyCode code) {
        mPressedKeyboardKeys.insert(code);
        mKeyboardEvent[KeyboardAction::KeyDown](this);
    }

    void Input::unregisterKey(KeyCode code) {
        mPressedKeyboardKeys.erase(code);
        mKeyboardEvent[KeyboardAction::KeyUp](this);
    }
    
    bool Input::isKeyPressed(Key key) const {
        using type = std::underlying_type<Key>::type;
        type rawKey = static_cast<type>(key);
        const auto& it = mPressedKeyboardKeys.find(rawKey);
        return it != mPressedKeyboardKeys.end();
    }
    
    bool Input::isMouseButtonPressed(uint8_t button) const {
        return (1 << button) & mPressedMouseButtonsMask;
    }
    
}
