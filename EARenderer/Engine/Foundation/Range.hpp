//
//  Range.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 12.10.2017.
//  Copyright © 2017 MPO. All rights reserved.
//

#ifndef Range_hpp
#define Range_hpp

#include <stdlib.h>

namespace EArenderer {
    
    struct Range {
        int64_t start;
        int64_t length;
        
        Range(int64_t start, int64_t length);
    };
    
}

#endif /* Range_hpp */
