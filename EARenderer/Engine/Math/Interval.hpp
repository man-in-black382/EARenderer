//
//  Interval.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 26.11.2017.
//  Copyright © 2017 MPO. All rights reserved.
//

#ifndef Interval_hpp
#define Interval_hpp

namespace EARenderer {
    
    struct Interval {
        float min = 0;
        float max = 0;
        
        Interval() = default;
        Interval(float min, float max);
    };
    
}

#endif /* Interval_hpp */
