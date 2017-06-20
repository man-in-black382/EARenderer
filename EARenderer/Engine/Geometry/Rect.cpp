//
//  Rect.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 16.06.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "Rect.hpp"

namespace EARenderer {
    
    const Rect& Rect::zero() {
        static Rect zero = { { 0, 0 }, { 0, 0 } };
        return zero;
    }
    
}
