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
    
    Size::Size(float w, float h)
    :
    width(w),
    height(h)
    { }
    
    bool Size::operator==(const Size& rhs) {
        return width == rhs.width && height == rhs.height;
    }
    
    const float Box::diagonal() const {
        return glm::length(max - min);
    }
    
}
