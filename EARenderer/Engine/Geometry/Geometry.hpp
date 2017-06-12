//
//  Rect.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 07.04.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#ifndef Rect_hpp
#define Rect_hpp

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

namespace EARenderer {
    
    struct Size {
        float width;
        float height;
        
        static const Size& zero();
        Size(float w, float h);
        
        bool operator==(const Size& rhs);
    };
    
    struct Rect {
        glm::vec2 origin;
        Size size;
        
        static const Rect& zero();
    };
    
    struct Box {
        glm::vec3 min;
        glm::vec3 max;
        
        const float diagonal() const;
    };
    
}

#endif /* Rect_hpp */
