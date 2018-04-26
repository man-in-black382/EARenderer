//
//  Throttle.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 08.10.2017.
//  Copyright © 2017 MPO. All rights reserved.
//

#ifndef Throttle_hpp
#define Throttle_hpp

#include <stdlib.h>
#include <chrono>
#include <functional>

namespace EARenderer {
    
    class Throttle {
    private:
        using TimePoint = std::chrono::time_point<std::chrono::high_resolution_clock>;
        
        TimePoint mPreviousFrameTime;
        float mCooldown = 0.0;
        
    public:
        using Action = std::function<void()>;
        
        Throttle(float cooldown);
        
        float cooldown() const;
        void setCooldown(float cooldown);
        
        /**
         Performs an action if enough time has passed since the last performed action

         @param action function object wich performs some work and needs to be throttled
         */
        void attemptToPerformAction(Action action);
    };
    
}

#endif /* Throttle_hpp */
