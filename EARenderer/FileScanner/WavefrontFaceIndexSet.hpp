//
//  WavefrontFaceIndexSet.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 09.03.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#ifndef WavefrontFaceIndexSet_hpp
#define WavefrontFaceIndexSet_hpp

#include <stdlib.h>

namespace EARenderer {
    
    struct WavefrontFaceIndexSet {
        int32_t positionIndex = 0;
        int32_t textureCorrdsIndex = 0;
        int32_t normalIndex = 0;
        
        WavefrontFaceIndexSet(int32_t p, int32_t uv, int32_t n);
    };
    
}

#endif /* WavefrontFaceIndexSet_hpp */
