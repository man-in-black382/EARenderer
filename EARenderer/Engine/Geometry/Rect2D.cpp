//
//  Rect.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 16.06.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "Rect2D.hpp"

namespace EARenderer {
    
    const Rect2D& Rect2D::zero() {
        static Rect2D zero;
        return zero;
    }
    
    Rect2D::Rect2D(const Size2D& size)
    :
    origin({ 0, 0 }),
    size(size)
    { }
    
    Rect2D::Rect2D(const glm::vec2& origin, const Size2D& size)
    :
    origin(origin),
    size(size)
    { }
    
}
