//
//  Input.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 11.06.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#ifndef Input_hpp
#define Input_hpp

#include <glm/vec2.hpp>

#include <unordered_set>
#include <string>
#include <type_traits>

#include "Event.inl"

namespace EARenderer {
    
    class Input {
    public:
        enum class KeyboardAction {
            KeyDown, KeyUp
        };
        
        enum class MouseAction {
            PressDown, PressUp, Drag, Move
        };
        
        using KeyCode = uint16_t;
        using KeySet = std::unordered_set<KeyCode>;
        using KeyboardEvent = MultiEvent<Input, KeyboardAction, std::string, void(const Input *)>;
        using MouseEvent = MultiEvent<Input, MouseAction, std::string, void(const Input *)>;
        
        enum class Key: KeyCode {
            W = 13, S = 1, A = 0, D = 2
        };
        
    private:
        MouseEvent mMouseEvent;
        KeyboardEvent mKeyboardEvent;
        
        glm::vec2 mMousePosition;
        KeyCode mPressedMouseButtonsMask;
        KeySet mPressedKeyboardKeys;
        
        Input() = default;
        ~Input() = default;
        
        Input(const Input& that) = delete;
        Input& operator=(const Input& rhs) = delete;
        
    public:
        static Input& shared();
        
        MouseEvent& mouseEvent();
        KeyboardEvent& keyboardEvent();
        const glm::vec2& mousePosition() const;
        const KeyCode pressedMouseButtonsMask() const;
        const KeySet& pressedKeyboardButtons() const;
        
        void registerMouseAction(MouseAction action, const glm::vec2& position, KeyCode keysMask);
        void registerKey(KeyCode code);
        void unregisterKey(KeyCode code);
        
        bool isKeyPressed(Key key) const;
        bool isMouseButtonPressed(uint8_t button) const;
    };
    
}

#endif /* Input_hpp */
