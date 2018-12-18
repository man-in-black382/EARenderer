//
//  Throttle.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 08.10.2017.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "Throttle.hpp"

namespace EARenderer {

#pragma mark - Lifecycle

    Throttle::Throttle(float cooldown)
            :
            mPreviousFrameTime(std::chrono::high_resolution_clock::now()),
            mCooldown(cooldown) {
    }

#pragma mark - Getters

    float Throttle::cooldown() const {
        return mCooldown;
    }

#pragma mark - Setters

    void Throttle::setCooldown(float cooldown) {
        mCooldown = cooldown;
    }

#pragma mark - Actions

    void Throttle::attemptToPerformAction(Action action) {
        auto now = std::chrono::high_resolution_clock::now();
        auto now_ms = std::chrono::time_point_cast<std::chrono::milliseconds>(now);
        auto previous_ms = std::chrono::time_point_cast<std::chrono::milliseconds>(mPreviousFrameTime);
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now_ms - previous_ms);

        if (elapsed.count() < mCooldown) {
            return;
        }

        action();
        mPreviousFrameTime = now;
    }

}
