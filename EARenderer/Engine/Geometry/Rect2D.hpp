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
    };
    
}

#endif /* Rect_hpp */
