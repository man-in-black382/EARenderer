//
//  WavefrontFaceIndexSet.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 09.03.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "WavefrontFaceIndexSet.hpp"

namespace EARenderer {
    
    #pragma mark - Lifecycle
    
    WavefrontFaceIndexSet::WavefrontFaceIndexSet(int32_t p, int32_t uv, int32_t n)
    :
    positionIndex(p),
    textureCorrdsIndex(uv),
    normalIndex(n)
    { }
    
}
