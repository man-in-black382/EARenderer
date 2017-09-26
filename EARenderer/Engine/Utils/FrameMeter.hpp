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
#include <chrono>

namespace EARenderer {
    
    class FrameMeter {
    public:
        struct FrameCharacteristics {
            double framesPerSecond;
            double frameTimeMillisecons;
        };
        
    private:
        using TimePoint = std::chrono::time_point<std::chrono::high_resolution_clock>;
        
        uint64_t mPassedFrames;
        TimePoint mPreviousFrameTime;
        FrameCharacteristics mPreviousCharacteristics;
        
    public:
        FrameCharacteristics tick(double throttleMilliseconds = 0.0);
    };
    
}

#endif /* FPSMeter_hpp */
