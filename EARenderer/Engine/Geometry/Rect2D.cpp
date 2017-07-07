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
        static Rect2D zero = { { 0, 0 }, { 0, 0 } };
        return zero;
    }
    
}
