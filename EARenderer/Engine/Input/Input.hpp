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

namespace EARenderer {
    
    class Input {
    public:
        enum class Direction : uint16_t {
            up = 13, down = 1, left = 0, right = 2
        };
        
        enum class MouseAction {
            pressDown, pressUp, drag, freeMovement
        };
        
    private:
        glm::vec2 mPreviousDraggedMousePosition;
        glm::vec2 mFreeMousePosition;
        glm::vec2 mDraggedMousePosition;
        std::unordered_set<uint16_t> mPressedKeyCodes;
        bool mShouldReturnDelta;
        
        Input();
        ~Input();
        
        Input(const Input& that) = delete;
        Input& operator=(const Input& rhs) = delete;
        
    public:
        static Input& shared();
        
        void updateMousePosition(const glm::vec2& position, MouseAction action);
        void registerKey(uint16_t code);
        void unregisterKey(uint16_t code);
        
        const glm::vec2& freeMousePosition() const;
        const glm::vec2& draggedMousePosition() const;
        const std::unordered_set<uint16_t>& pressedKeyCodes() const;
        
        glm::vec2 draggedMouseDelta();
        bool isDirectionKeyPressed(Direction direction) const;
    };
    
}

#endif /* Input_hpp */
