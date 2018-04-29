//
//  FPSMeter.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 26.09.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "FrameMeter.hpp"

namespace EARenderer {
    
    #pragma mark - Lifecycle
    
    FrameMeter::FrameMeter(float throttleMilliseconds)
    :
    mThrottle(throttleMilliseconds)
    { }
    
#pragma mark - Public methods
    
    FrameMeter::FrameCharacteristics FrameMeter::tick() {
        mPassedFrames++;
        mThrottle.attemptToPerformAction([this]() {
            mFrameCharacteristics.framesPerSecond = 1000.f / mThrottle.cooldown() * mPassedFrames;
            mFrameCharacteristics.frameTimeMillisecons = mThrottle.cooldown() / mPassedFrames;
            mPassedFrames = 0;
        });
        return mFrameCharacteristics;
    }
    
}
