//
//  FPSMeter.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 26.09.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#ifndef FPSMeter_hpp
#define FPSMeter_hpp

#include <stdlib.h>

#include "Throttle.hpp"

namespace EARenderer {
    
    class FrameMeter {
    public:
        struct FrameCharacteristics {
            double framesPerSecond;
            double frameTimeMillisecons;
        };
        
    private:
        uint64_t mPassedFrames;
        FrameCharacteristics mFrameCharacteristics;
        Throttle mThrottle;
        
    public:
        FrameMeter(float throttleMilliseconds = 200);
        
        FrameCharacteristics tick();
    };
    
}

#endif /* FPSMeter_hpp */
