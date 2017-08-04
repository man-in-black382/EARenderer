//
//  Light.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 25.03.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#ifndef Light_hpp
#define Light_hpp

#include "Color.hpp"

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

namespace EARenderer {
    
    class Light {
    protected:
        glm::vec3 mPosition;
        Color mColor;
        
    public:
        Light();
        Light(const glm::vec3& position, const Color& color);
        
        const glm::vec3& position() const;
        const Color& color() const;
        
        void setPosition(const glm::vec3& position);
        void setColor(const Color& color);
    };
    
}

#endif /* Light_hpp */
