//
//  FPSMeter.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 26.09.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "FrameMeter.hpp"

#include <chrono>

#include <stdio.h>

namespace EARenderer {
    
    FrameMeter::FrameCharacteristics FrameMeter::tick(double throttleMilliseconds) {
        auto now = std::chrono::high_resolution_clock::now();
        auto now_ms = std::chrono::time_point_cast<std::chrono::milliseconds>(now);
        auto previous_ms = std::chrono::time_point_cast<std::chrono::milliseconds>(mPreviousFrameTime);
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now_ms - previous_ms);
        
        mPassedFrames++;
        
        if (elapsed.count() < throttleMilliseconds) {
            return mPreviousCharacteristics;
        }
        
        FrameCharacteristics c;
        c.framesPerSecond = 1000.f / static_cast<double>(elapsed.count()) * mPassedFrames;
        c.frameTimeMillisecons = static_cast<double>(elapsed.count()) / mPassedFrames;
        
        mPreviousCharacteristics = c;
        mPreviousFrameTime = now;
        mPassedFrames = 0;
        
        return c;
    }
    
}
