//
//  Rect.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 16.06.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#ifndef Rect_hpp
#define Rect_hpp

#include "Size2D.hpp"
#include <glm/vec2.hpp>

namespace EARenderer {
    
    struct Rect2D {
        glm::vec2 origin;
        Size2D size;
        
        static const Rect2D& zero();
        
        Rect2D() = default;
        Rect2D(const Size2D& size);
        Rect2D(const glm::vec2& origin, const Size2D& size);

        float minX() const;
        float minY() const;
        float maxX() const;
        float maxY() const;
    };
    
}

#endif /* Rect_hpp */
