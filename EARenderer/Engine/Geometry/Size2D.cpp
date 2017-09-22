//
//  Size.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 16.06.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "Size2D.hpp"

namespace EARenderer {
    
    const Size2D& Size2D::zero() {
        static Size2D zero = { 0, 0 };
        return zero;
    }
    
    Size2D::Size2D(float w, float h)
    :
    width(w),
    height(h)
    { }
    
    bool Size2D::operator==(const Size2D& rhs) {
        return width == rhs.width && height == rhs.height;
    }
    
    bool Size2D::operator!=(const Size2D& rhs) {
        return !operator==(rhs);
    }
    
}