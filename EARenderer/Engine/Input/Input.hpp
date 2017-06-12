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
    
    enum class InputDirection : uint16_t {
        up = 13, down = 1, left = 0, right = 2
    };
    
    class Input {
    private:
        glm::vec2 mPreviousMousePosition;
        glm::vec2 mMousePosition;
        std::unordered_set<uint16_t> mPressedKeyCodes;
        bool mShouldReturnDelta;
        
        Input();
        ~Input();
        
        Input(const Input& that) = delete;
        Input& operator=(const Input& rhs) = delete;
        
    public:
        static Input& shared();
        
        void updateMousePosition(const glm::vec2& position, bool dropPreviousPosition);
        void registerKey(uint16_t code);
        void unregisterKey(uint16_t code);
        bool isDirectionKeyPressed(InputDirection direction);
        
        glm::vec2 mouseDelta();
        const glm::vec2& mousePosition() const;
        const std::unordered_set<uint16_t>& pressedKeyCodes() const;
    };
    
}

#endif /* Input_hpp */
