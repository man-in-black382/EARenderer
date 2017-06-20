//
//  Rect.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 16.06.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#ifndef Rect_hpp
#define Rect_hpp

#include "Size.hpp"
#include <glm/vec2.hpp>

namespace EARenderer {
    
    struct Rect {
        glm::vec2 origin;
        Size size;
        
        static const Rect& zero();
    };
    
}

#endif /* Rect_hpp */
