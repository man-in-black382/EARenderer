//
//  Rect.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 07.04.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "Geometry.hpp"

#include <glm/gtx/norm.hpp>

namespace EARenderer {
    
#pragma mark - Size
    
    const Size& Size::zero() {
        static Size zero = { 0, 0 };
        return zero;
    }
    
    Size::Size(float w, float h)
    :
    width(w),
    height(h)
    { }
    
    bool Size::operator==(const Size& rhs) {
        return width == rhs.width && height == rhs.height;
    }
    
#pragma mark - Rect
    
    const Rect& Rect::zero() {
        static Rect zero = { { 0, 0 }, { 0, 0 } };
        return zero;
    }
    
#pragma mark - Box
    
    const float Box::diagonal() const {
        return glm::length(max - min);
    }
    
}
