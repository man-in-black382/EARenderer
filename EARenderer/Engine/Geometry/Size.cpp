//
//  Size.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 16.06.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "Size.hpp"

namespace EARenderer {
    
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
    
    bool Size::operator!=(const Size& rhs) {
        return !operator==(rhs);
    }
    
}
